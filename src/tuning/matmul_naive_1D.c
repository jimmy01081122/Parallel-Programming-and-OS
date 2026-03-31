// matmul_naive_1D.c
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000

void init_1d(double *mat, int size) {
    for (int i = 0; i < size * size; i++) {
        mat[i] = (double)rand() / RAND_MAX;
    }
}

int main() {
    // 1. 分配連續記憶體
    double *A = (double *)malloc(N * N * sizeof(double));
    double *B = (double *)malloc(N * N * sizeof(double));
    double *C = (double *)calloc(N * N, sizeof(double));

    srand(time(NULL));
    init_1d(A, N);
    init_1d(B, N);

    printf("--- 1D 連續陣列版本 (N=%d) ---\n", N);
    double start = omp_get_wtime();

    // 2. 核心計算
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++) {
                // 依賴暫存器計算位址，沒有額外的指標查找
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }

    double end = omp_get_wtime();
    printf("1D 版本耗時: %.4f 秒\n\n", end - start);

    free(A); free(B); free(C);
    return 0;
}