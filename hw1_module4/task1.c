#include <stdio.h>


void swap(int *a, int *b) {
    int t;
    t = *a;
    *a = *b;
    *b = t;
}

int main() {
    int x, y;

    scanf("%d", &x);
    scanf("%d", &y);
    swap(&x, &y);
    printf("swaped x = %d, y = %d\n", x, y);

    return 0;
}
