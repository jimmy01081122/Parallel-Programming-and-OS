// matmul_naive_2D.c
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000

// 分配不連續的指標陣列
double** alloc_2d(int n) {
    double **mat = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        mat[i] = (double *)malloc(n * sizeof(double));
    }
    return mat;
}

void free_2d(double **mat, int n) {
    for (int i = 0; i < n; i++) free(mat[i]);
    free(mat);
}

void init_2d(double **mat, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            mat[i][j] = (double)rand() / RAND_MAX;
        }
    }
}

int main() {
    // 1. 分配分散的記憶體 (Array of Pointers)
    double **A = alloc_2d(N);
    double **B = alloc_2d(N);
    double **C = alloc_2d(N);

    // C 矩陣需要手動歸零
    for(int i=0; i<N; i++) {
        for(int j=0; j<N; j++) C[i][j] = 0.0;
    }

    srand(time(NULL));
    init_2d(A, N);
    init_2d(B, N);

    printf("--- 2D 指標陣列版本 (N=%d) ---\n", N);
    double start = omp_get_wtime();
                // 隱含雙重位址查找：先去記憶體找 A[i] 的位址，再去抓 [k] 的值
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;

    // 2. 核心計算
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++) {
        }
    }

    double end = omp_get_wtime();
    printf("2D 版本耗時: %.4f 秒\n\n", end - start);

    free_2d(A, N); free_2d(B, N); free_2d(C, N);
    return 0;
}