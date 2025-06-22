#include "malloc.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "trace.h"

#define PTR_ADD(p, offset) (((char *) p) + offset)
#define PTR_SUB(p, offset) (((char *) p) - offset)

typedef unsigned int word_t;

#define WSIZE sizeof(word_t)
#define DSIZE (WSIZE << 1)

#define MINBLOCKSIZE (WSIZE + DSIZE + DSIZE + WSIZE)
#define MINBLOCKSIZE_ALIGNED 32

#define GET(p) (*(word_t *)(p))
#define PUT(p, val) (*(word_t *)(p) = (word_t)(val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_PREV_ALLOC(p) (GET(p) & 0x2)

#define SET_SIZE_ALLOC(p, size, alloc) (PUT(p, (size) | (alloc)))
#define SET_PREV_ALLOC(p, prev_alloc) (PUT(p, GET(p) | ((prev_alloc) << 1)))

#define ALIGNED_SIZE(size, unit) (unit * ((size + (unit - 1)) / unit))

typedef struct free_block {
    struct free_block *prev;
    struct free_block *next;
} free_block_t;

typedef struct {
    int lock;
    size_t pagesize;
    void *start;
    void *end;
    void *head;
    free_block_t *free_list_head;
    void *next_fit_pos;
    size_t alloc;
    size_t payload;
    size_t usage;
} heap_t;

static heap_t g_heap = {0};

#define HEAP_LOCK while(__atomic_test_and_set(&g_heap.lock, __ATOMIC_ACQUIRE)) {}
#define HEAP_UNLOCK __atomic_clear(&g_heap.lock, __ATOMIC_RELEASE);

static void free_list_insert(free_block_t *block) {
    if (g_heap.free_list_head) {
        g_heap.free_list_head->prev = block;
    }
    block->next = g_heap.free_list_head;
    block->prev = NULL;
    g_heap.free_list_head = block;
}

static void free_list_remove(free_block_t *block) {
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        g_heap.free_list_head = block->next;
    }
    if (block->next) {
        block->next->prev = block->prev;
    }
}

void heap_dump() {
    void *hdr = PTR_SUB(g_heap.head, WSIZE);
    word_t hdata = GET(hdr);
    size_t hsize;
    char buff[64] = {0};
    while ((hsize = (hdata & ~0x7))) {
        void *ptr = PTR_ADD(hdr, WSIZE);
        int len = snprintf(buff, sizeof(buff), "%p = [%zu/%d]\n", ptr, hsize, hdata & 1);
        write(STDOUT_FILENO, buff, len);
        hdr = PTR_ADD(hdr, hsize);
        hdata = GET(hdr);
    }
}

void stat_dump() {
    size_t heapsize = g_heap.end - g_heap.start;
    size_t allocated = g_heap.alloc;
    size_t free = heapsize - allocated;
    long usage = (100 * allocated) / heapsize;
    long payload = (g_heap.payload * 100) / g_heap.usage;
    char buff[256];
    int len = snprintf(buff, sizeof(buff), "Heap = %zu\nAlloc = %zu\nFree = %zu\nUsage = %ld%%\nPayload = %ld%%\n", heapsize, allocated, free, usage, payload);
    write(STDOUT_FILENO, buff, len);
}

static void heap_init() {
    heap_t heap = {0};
    heap.pagesize = sysconf(_SC_PAGESIZE);
    heap.start = sbrk(heap.pagesize);
    heap.end = PTR_ADD(heap.start, heap.pagesize);
    heap.head = PTR_ADD(heap.start, DSIZE);
    heap.next_fit_pos = heap.head;
    size_t bsize = heap.pagesize - DSIZE;
    
    PUT(heap.start, 1);
    PUT(PTR_ADD(heap.start, WSIZE), bsize);
    PUT(PTR_SUB(heap.end, DSIZE), bsize);
    PUT(PTR_SUB(heap.end, WSIZE), 1);
    
    SET_PREV_ALLOC(PTR_ADD(heap.start, WSIZE), 1);
    
    free_block_t *free_block = (free_block_t *)(PTR_ADD(heap.start, DSIZE));
    free_block->prev = NULL;
    free_block->next = NULL;
    heap.free_list_head = free_block;
    
    g_heap = heap;
    trace("malloc is initialized\nstart=%p\nend= %p\nhead= %p\n", heap.start, heap.end, heap.head);
}

static void *heap_extend(size_t size) {
    size_t asize = ALIGNED_SIZE(size, g_heap.pagesize);
    void *start = sbrk(asize);
    void *end = PTR_ADD(start, asize);
    PUT(PTR_SUB(start, WSIZE), asize);
    PUT(PTR_SUB(end, DSIZE), asize);
    PUT(PTR_SUB(end, WSIZE), 1);
    
    SET_PREV_ALLOC(PTR_SUB(start, WSIZE), 1);
    
    g_heap.end = end;
    return start;
}

static void place(void *ptr, size_t asize) {
    void *hdr = PTR_SUB(ptr, WSIZE);
    size_t bsize = GET_SIZE(hdr);
    size_t rsize = bsize - asize;
    
    if (rsize >= MINBLOCKSIZE_ALIGNED) {
        void *rhdr = PTR_ADD(hdr, asize);
        PUT(hdr, asize | 1);
        PUT(rhdr, rsize);
        PUT(PTR_ADD(rhdr, rsize - WSIZE), rsize);
        
        void *next_next_hdr = PTR_ADD(rhdr, rsize);
        SET_PREV_ALLOC(next_next_hdr, 0);
        
        free_block_t *free_block = (free_block_t *)(PTR_ADD(rhdr, WSIZE));
        free_list_insert(free_block);
        
        g_heap.next_fit_pos = PTR_ADD(hdr, asize);
    } else {
        PUT(hdr, bsize | 1);
        g_heap.next_fit_pos = PTR_ADD(hdr, bsize);
    }
}

static void *coalesce(void *ptr) {
    void *hdr = PTR_SUB(ptr, WSIZE);
    size_t size = GET_SIZE(hdr);
    word_t prev_alloc = GET_PREV_ALLOC(hdr);
    void *next_hdr = PTR_ADD(hdr, size);
    word_t next_alloc = GET_ALLOC(next_hdr);
    
    if (prev_alloc && next_alloc) {
        return ptr;
    }
    
    size_t prev_size = 0;
    void *prev_hdr = NULL;
    if (!prev_alloc) {
        void *curr = PTR_SUB(hdr, WSIZE);
        while (curr > g_heap.start) {
            curr = PTR_SUB(curr, WSIZE);
            if (GET_SIZE(curr) > 0) {
                prev_size = GET_SIZE(curr);
                prev_hdr = curr;
                break;
            }
        }
    }
    
    size_t next_size = GET_SIZE(next_hdr);
    void *next_ftr = PTR_ADD(next_hdr, next_size - WSIZE);
    
    if (prev_alloc && !next_alloc) {
        free_block_t *next_free = (free_block_t *)(PTR_ADD(next_hdr, WSIZE));
        free_list_remove(next_free);
        
        size += next_size;
        PUT(hdr, size);
        PUT(next_ftr, size);
        
        free_list_insert((free_block_t *)ptr);
    } else if (!prev_alloc && next_alloc) {
        free_block_t *prev_free = (free_block_t *)(PTR_ADD(prev_hdr, WSIZE));
        free_list_remove(prev_free);
        
        size += prev_size;
        PUT(prev_hdr, size);
        PUT(PTR_ADD(prev_hdr, size - WSIZE), size);
        ptr = PTR_ADD(prev_hdr, WSIZE);
        
        free_list_insert((free_block_t *)ptr);
    } else {
        free_block_t *prev_free = (free_block_t *)(PTR_ADD(prev_hdr, WSIZE));
        free_block_t *next_free = (free_block_t *)(PTR_ADD(next_hdr, WSIZE));
        free_list_remove(prev_free);
        free_list_remove(next_free);
        
        size += prev_size + next_size;
        PUT(prev_hdr, size);
        PUT(PTR_ADD(prev_hdr, size - WSIZE), size);
        ptr = PTR_ADD(prev_hdr, WSIZE);
        
        free_list_insert((free_block_t *)ptr);
    }
    return ptr;
}

static void *find_fit(size_t size) {
    if (g_heap.head == NULL) {
        heap_init();
    }
    
    for (free_block_t *fb = g_heap.free_list_head; fb != NULL; fb = fb->next) {
        void *hdr = PTR_SUB(fb, WSIZE);
        if (GET_SIZE(hdr) >= size) {
            g_heap.next_fit_pos = PTR_ADD(hdr, GET_SIZE(hdr));
            return fb;
        }
    }
    
    return NULL;
}

static void *realloc_improved(void *ptr, size_t size) {
    if (!ptr) {
        return malloc(size);
    }
    
    void *hdr = PTR_SUB(ptr, WSIZE);
    size_t old_size = GET_SIZE(hdr) - DSIZE;
    
    if (size <= old_size) {
        size_t asize = ALIGNED_SIZE(size, DSIZE) + DSIZE;
        size_t rsize = GET_SIZE(hdr) - asize;
        
        if (rsize >= MINBLOCKSIZE_ALIGNED) {
            void *rhdr = PTR_ADD(hdr, asize);
            PUT(hdr, asize | 1);
            PUT(rhdr, rsize);
            PUT(PTR_ADD(rhdr, rsize - WSIZE), rsize);
            
            void *next_next_hdr = PTR_ADD(rhdr, rsize);
            SET_PREV_ALLOC(next_next_hdr, 0);
            
            free_block_t *free_block = (free_block_t *)(PTR_ADD(rhdr, WSIZE));
            free_list_insert(free_block);
            
            g_heap.alloc -= rsize;
            g_heap.usage -= rsize;
            
            g_heap.next_fit_pos = PTR_ADD(hdr, asize);
        }
        return ptr;
    }
    
    void *next_hdr = PTR_ADD(hdr, GET_SIZE(hdr));
    if (!GET_ALLOC(next_hdr)) {
        size_t combined_size = GET_SIZE(hdr) + GET_SIZE(next_hdr);
        size_t asize = ALIGNED_SIZE(size, DSIZE) + DSIZE;
        
        if (combined_size >= asize) {
            free_block_t *next_free = (free_block_t *)(PTR_ADD(next_hdr, WSIZE));
            free_list_remove(next_free);
            
            PUT(hdr, combined_size | 1);
            g_heap.alloc += GET_SIZE(next_hdr);
            g_heap.usage += GET_SIZE(next_hdr);
            
            size_t rsize = combined_size - asize;
            if (rsize >= MINBLOCKSIZE_ALIGNED) {
                void *rhdr = PTR_ADD(hdr, asize);
                PUT(hdr, asize | 1);
                PUT(rhdr, rsize);
                PUT(PTR_ADD(rhdr, rsize - WSIZE), rsize);
                
                void *next_next_hdr = PTR_ADD(rhdr, rsize);
                SET_PREV_ALLOC(next_next_hdr, 0);
                
                free_block_t *free_block = (free_block_t *)(PTR_ADD(rhdr, WSIZE));
                free_list_insert(free_block);
                
                g_heap.alloc -= rsize;
                g_heap.usage -= rsize;
                
                g_heap.next_fit_pos = PTR_ADD(hdr, asize);
            } else {
                g_heap.next_fit_pos = PTR_ADD(hdr, combined_size);
            }
            return ptr;
        }
    }
    
    void *newptr = malloc(size);
    if (newptr) {
        memcpy(newptr, ptr, old_size);
        free(ptr);
    }
    return newptr;
}

void *malloc(size_t size) {
    trace("malloc(%zu)\n", size);
    if (size == 0) {
        return NULL;
    }
    
    size_t asize = ALIGNED_SIZE(size, DSIZE) + DSIZE;
    if (asize < MINBLOCKSIZE_ALIGNED) {
        asize = MINBLOCKSIZE_ALIGNED;
    }
    
    HEAP_LOCK
    void *ptr = find_fit(asize);
    if (!ptr) {
        ptr = heap_extend(asize);
        ptr = coalesce(ptr);
    }
    
    void *hdr = PTR_SUB(ptr, WSIZE);
    if (!GET_ALLOC(hdr)) {
        free_block_t *free_block = (free_block_t *)ptr;
        free_list_remove(free_block);
    }
    
    place(ptr, asize);
    g_heap.alloc += asize;
    g_heap.usage += asize;
    g_heap.payload += size;
    HEAP_UNLOCK
    
    trace("malloc(%zu) = %p\n", size, ptr);
    return ptr;
}

void free(void *ptr) {
    trace("free(%p)\n", ptr);
    if (!ptr) {
        return;
    }
    
    void *hdr = PTR_SUB(ptr, WSIZE);
    HEAP_LOCK
    size_t size = GET_SIZE(hdr);
    
    PUT(hdr, size);
    PUT(PTR_ADD(hdr, size - WSIZE), size);
    
    void *next_hdr = PTR_ADD(hdr, size);
    SET_PREV_ALLOC(next_hdr, 0);
    
    free_block_t *free_block = (free_block_t *)ptr;
    free_list_insert(free_block);
    
    coalesce(ptr);
    g_heap.alloc -= size;
    HEAP_UNLOCK
}

void *calloc(size_t nmemb, size_t size) {
    trace("calloc(%zu, %zu)\n", nmemb, size);
    size_t asize = nmemb * size;
    if (asize / size != nmemb) {
        errno = ENOMEM;
        return NULL;
    }
    void *ptr = malloc(asize);
    if (ptr) {
        memset(ptr, 0, asize);
    }
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    trace("realloc(%p, %zu)\n", ptr, size);
    HEAP_LOCK
    void *result = realloc_improved(ptr, size);
    HEAP_UNLOCK
    return result;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size) {
    size_t asize = nmemb * size;
    if (asize / size != nmemb) {
        errno = ENOMEM;
        return NULL;
    }
    return realloc(ptr, asize);
} 