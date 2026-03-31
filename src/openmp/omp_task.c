#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


long fib_serial(int n) {
    if (n < 2) return n;
    return fib_serial(n - 1) + fib_serial(n - 2);
}

// 平行任務版本
long fib_task(int n, int cutoff) {
    if (n < 2) return n;
    
    // 當小於 cutoff 時，停止產生 Task，改用序列計算
    if (n < cutoff) {
        return fib_serial(n);
    }

    long x, y;
    #pragma omp task shared(x)
    x = fib_task(n - 1, cutoff);

    #pragma omp task shared(y)
    y = fib_task(n - 2, cutoff);

    #pragma omp taskwait
    return x + y;
}

int main() {
    int n, cutoff;
    printf("請輸入 Fibonacci 項數 (建議 30-40): ");
    if (scanf("%d", &n) != 1) return 1;
    printf("請輸入 Task Cutoff 閾值 (建議 20，輸入 0 代表全平行): ");
    if (scanf("%d", &cutoff) != 1) return 1;

    double start, end;
    long result;

    // --- 1. 測量純序列時間 ---
    printf("\n[1] 執行純序列計算...\n");
    start = omp_get_wtime();
    result = fib_serial(n);
    end = omp_get_wtime();
    printf("序列結果: %ld | 耗時: %f 秒\n", result, end - start);

    // --- 2. 測量平行任務時間 ---
    printf("\n[2] 執行 OpenMP Task 計算 (Cutoff=%d)...\n", cutoff);
    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        result = fib_task(n, cutoff);
    }
    end = omp_get_wtime();
    printf("平行結果: %ld | 耗時: %f 秒\n", result, end - start);

    return 0;
}