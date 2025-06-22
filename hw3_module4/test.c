#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Testing my malloc implementation\n");
    
    int *p1 = malloc(100);
    printf("Got 100 bytes at %p\n", p1);
    
    p1 = realloc(p1, 50);
    printf("Made it smaller: %p\n", p1);
    
    p1 = realloc(p1, 200);
    printf("Made it bigger: %p\n", p1);
    
    int *p2 = malloc(64);
    int *p3 = malloc(128);
    printf("Got more blocks: %p, %p\n", p2, p3);
    
    free(p1);
    free(p2);
    free(p3);
    printf("Freed everything\n");
    
    return 0;
} 