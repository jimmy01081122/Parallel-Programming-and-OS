// matmul_naive_1TD.c
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define THREADS 1
#define N 1000

void init_1d(double *mat, int size) {
    for (int i = 0; i < size * size; i++) {
        mat[i] = (double)rand() / RAND_MAX;
    }
}

int main() {
    double *A = (double *)malloc(N * N * sizeof(double));
    double *B = (double *)malloc(N * N * sizeof(double));
    double *B_T = (double *)malloc(N * N * sizeof(double)); // 用於存放轉置後的 B
    double *C = (double *)calloc(N * N, sizeof(double));

    srand(time(NULL));
    init_1d(A, N);
    init_1d(B, N);

    printf("--- 1D 轉置優化版本 (N=%d) ---\n", N);
    double start = omp_get_wtime();

    // 1. 矩陣轉置階段 (這部分開銷極小，但收益巨大)
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            B_T[i * N + j] = B[j * N + i];
        }
    }

    // 2. 核心計算階段
    #pragma omp parallel for num_threads(THREADS) collapse(2) 
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++) {
                // 現在 A[i*N + k] 和 B_T[j*N + k] 都是連續記憶體存取！
                sum += A[i * N + k] * B_T[j * N + k];
            }
            C[i * N + j] = sum;
        }
    }

    double end = omp_get_wtime();
    printf("轉置版本耗時: %.4f 秒\n", end - start);

    free(A); free(B); free(B_T); free(C);
    return 0;
}