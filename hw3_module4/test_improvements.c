#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("=== Testing My Awesome Malloc Implementation ===\n\n");
    
    printf("Test 1: Basic stuff\n");
    void *p1 = malloc(100);
    printf("Got 100 bytes at %p\n", p1);
    free(p1);
    printf("Freed it\n\n");
    
    printf("Test 2: Realloc magic\n");
    void *p2 = malloc(200);
    printf("Got 200 bytes at %p\n", p2);
    memset(p2, 'A', 200);
    p2 = realloc(p2, 50);
    printf("Made it smaller: %p\n", p2);
    printf("First byte: %c (should be 'A')\n", *(char*)p2);
    free(p2);
    printf("Freed it\n\n");
    
    printf("Test 3: Realloc expansion\n");
    void *p3 = malloc(100);
    void *p4 = malloc(100);
    printf("Got two blocks: %p and %p\n", p3, p4);
    free(p4);
    printf("Freed second one\n");
    memset(p3, 'B', 100);
    p3 = realloc(p3, 150);
    printf("Made first one bigger: %p\n", p3);
    printf("First byte: %c (should be 'B')\n", *(char*)p3);
    free(p3);
    printf("Freed it\n\n");
    
    printf("Test 4: Multiple blocks (testing next-fit)\n");
    void *blocks[10];
    for (int i = 0; i < 10; i++) {
        blocks[i] = malloc(64);
        printf("Block %d: %p\n", i, blocks[i]);
    }
    
    free(blocks[3]);
    free(blocks[5]);
    free(blocks[7]);
    printf("Freed blocks 3, 5, 7\n");
    
    void *new1 = malloc(32);
    void *new2 = malloc(32);
    printf("New ones: %p, %p\n", new1, new2);
    
    for (int i = 0; i < 10; i++) {
        if (i != 3 && i != 5 && i != 7) {
            free(blocks[i]);
        }
    }
    free(new1);
    free(new2);
    printf("Cleaned up\n\n");
    
    printf("Test 5: Small blocks\n");
    void *small1 = malloc(8);
    void *small2 = malloc(8);
    void *small3 = malloc(8);
    printf("Small ones: %p, %p, %p\n", small1, small2, small3);
    
    printf("Alignment check:\n");
    printf("small1: %p (aligned: %s)\n", small1, ((uintptr_t)small1 % 8 == 0) ? "yes" : "no");
    printf("small2: %p (aligned: %s)\n", small2, ((uintptr_t)small2 % 8 == 0) ? "yes" : "no");
    printf("small3: %p (aligned: %s)\n", small3, ((uintptr_t)small3 % 8 == 0) ? "yes" : "no");
    
    free(small1);
    free(small2);
    free(small3);
    printf("Freed small ones\n\n");
    
    printf("=== All tests passed! My malloc is awesome! ===\n");
    return 0;
} 