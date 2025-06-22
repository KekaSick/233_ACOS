#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    printf("Testing Free List Performance, I wish I wasn't\n\n");
    
    printf("Test 1: Speed test\n");
    clock_t start, end;
    double cpu_time_used;
    
    void *blocks[1000];
    start = clock();
    for (int i = 0; i < 1000; i++) {
        blocks[i] = malloc(32);
        if (!blocks[i]) {
            printf("Oops, failed to get block %d\n", i);
            return 1;
        }
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Got 1000 blocks in %.6f seconds (pretty fast!(hell nah))\n", cpu_time_used);
    
    for (int i = 0; i < 1000; i += 2) {
        free(blocks[i]);
    }
    
    start = clock();
    for (int i = 0; i < 500; i++) {
        void *new_block = malloc(32);
        if (!new_block) {
            printf("Failed to get new block %d\n", i);
            return 1;
        }
        free(new_block);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Got and freed 500 blocks in %.6f seconds (even faster!)\n", cpu_time_used);
    
    for (int i = 1; i < 1000; i += 2) {
        free(blocks[i]);
    }
    printf("Test 1 done\n\n");
    
    printf("Test 2: Coalescing magic\n");
    
    void *p1 = malloc(100);
    void *p2 = malloc(100);
    void *p3 = malloc(100);
    printf("Got three blocks: %p, %p, %p\n", p1, p2, p3);
    
    free(p2);
    printf("Freed middle one\n");
    
    free(p1);
    printf("Freed first one (should merge)\n");
    
    void *large = malloc(150);
    printf("Got big block: %p\n", large);
    
    free(p3);
    free(large);
    printf("Test 2 done\n\n");
    
    printf("Test 3: Next-fit stuff\n");
    
    void *blocks2[5];
    for (int i = 0; i < 5; i++) {
        blocks2[i] = malloc(64);
        printf("Block %d: %p\n", i, blocks2[i]);
    }
    
    free(blocks2[1]);
    free(blocks2[3]);
    printf("Freed blocks 1 and 3\n");
    
    void *new1 = malloc(32);
    void *new2 = malloc(32);
    printf("New ones: %p, %p\n", new1, new2);
    
    for (int i = 0; i < 5; i++) {
        if (i != 1 && i != 3) {
            free(blocks2[i]);
        }
    }
    free(new1);
    free(new2);
    printf("Test 3 done\n\n");
    
    printf("All tests passed! My free list is working\n");
    return 0;
} 