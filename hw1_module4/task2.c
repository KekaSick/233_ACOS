#include <stdio.h>
#include <stdlib.h>

int main() {
    int N;
    scanf("%d", &N);

    int *arr = (int *)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        scanf("%d", &arr[i]);
    }

    for (int i = 0; i < N / 2; i++) {
        int t = arr[i];
        arr[i] = arr[N - 1 - i];
        arr[N - 1 - i] = t;
    }

    for (int i = 0; i < N; i++) {
        printf("%d ", arr[i]);
    }
    
    printf("\n");
    free(arr);

    return 0;
}
