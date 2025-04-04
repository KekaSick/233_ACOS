#include <stdio.h>
#include <stdlib.h>

int main() {
    int cap = 10;
    int *arr = (int *)malloc(cap * sizeof(int));
    int s = 0;
    int x;

    while (1) {
        scanf("%d", &x);
        if (x == 0) break;
        if (s >= cap) {
            cap *= 2;
            arr = (int *)realloc(arr, cap * sizeof(int));
        }
        arr[s++] = x;
    }

    for (int i = 0; i < s / 2; i++) {
        int temp = arr[i];
        arr[i] = arr[s - 1 - i];
        arr[s - 1 - i] = temp;
    }

    printf("Reversed list:\n");
    for (int i = 0; i < s; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);

    return 0;
}
