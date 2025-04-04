#include <stdio.h>
#include <stdlib.h>

int main() {
    int N, M;

    printf("Enter number of rows: ");
    scanf("%d", &N);
    printf("Enter number of columns: ");
    scanf("%d", &M);

    int **mat = malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        mat[i] = malloc(M * sizeof(int));
    }

    printf("Enter %d values:\n", N * M);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            scanf("%d", &mat[i][j]);

    int **transpose = malloc(M * sizeof(int *));
    for (int i = 0; i < M; i++) {
        transpose[i] = malloc(N * sizeof(int));
    }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            transpose[j][i] = mat[i][j];

    printf("Transposed matrix:\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", transpose[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < N; i++) free(mat[i]);
    free(mat);

    for (int i = 0; i < M; i++) free(transpose[i]);
    free(transpose);

    return 0;
}
