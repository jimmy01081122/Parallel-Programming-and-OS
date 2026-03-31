#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

// 定義矩陣大小與分塊大小
#define N 1000
#define B 64  // Block Size: 根據 L1 Cache 大小調整 (32x32 double 約 8KB)

// 輔助函式：取最小值
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void init_matrix(double *mat, int size) {
    for (int i = 0; i < size * size; i++) {
        mat[i] = (double)rand() / RAND_MAX;
    }
}

int main() {
    // 1. 記憶體分配 (1D 連續空間)
    double *A   = (double *)malloc(N * N * sizeof(double));
    double *B_m = (double *)malloc(N * N * sizeof(double)); // 原始 B
    double *B_T = (double *)malloc(N * N * sizeof(double)); // 轉置後的 B
    double *C   = (double *)calloc(N * N, sizeof(double));  // 初始化為 0

    if (!A || !B_m || !B_T || !C) {
        printf("記憶體分配失敗！\n");
        return -1;
    }

    srand(time(NULL));
    init_matrix(A, N);
    init_matrix(B_m, N);

    printf("--- 最後優化版本: 1D + Transpose + Tiling (N=%d, B=%d) ---\n", N, B);
    double start_time = omp_get_wtime();

    // 2. 矩陣轉置 (將 B 轉置為 B_T，確保計算時為行連續存取)
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            B_T[i * N + j] = B_m[j * N + i];
        }
    }

    // 3. 核心計算：六層迴圈 (Tiled Matrix Multiplication)
    // 外三層迴圈：以「塊」為單位移動
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i += B) {
        for (int j = 0; j < N; j += B) {
            for (int k = 0; k < N; k += B) {
                
                // 內三層迴圈：在「塊」內部進行計算
                for (int ii = i; ii < MIN(i + B, N); ii++) {
                    for (int jj = j; jj < MIN(j + B, N); jj++) {
                        double sum = 0;
                        // 這裡 A 和 B_T 都是 Row-major 連續存取，極利於 Cache L1 與 SIMD
                        for (int kk = k; kk < MIN(k + B, N); kk++) {
                            sum += A[ii * N + kk] * B_T[jj * N + kk];
                        }
                        C[ii * N + jj] += sum;
                    }
                }

            }
        }
    }

    double end_time = omp_get_wtime();
    printf("耗時: %.4f 秒\n", end_time - start_time);

    // 驗證計算（可選，抽取一個值檢查）
    // printf("C[500]: %f\n", C[500]);

    free(A);
    free(B_m);
    free(B_T);
    free(C);

    return 0;
}