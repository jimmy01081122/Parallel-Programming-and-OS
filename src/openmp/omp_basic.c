// omp_basic.c
#include <omp.h>
#include <stdio.h>

#define TEST 0
int main() {
#if !TEST
    int total_sum = 0;
    int data[100];
    for(int i=0; i<100; i++) data[i] = 1;

    // 只需要這一行，編譯器就會幫你完成 Fork, Work-sharing, 與 Reduction
    #pragma omp parallel for reduction(+:total_sum)
    for (int i = 0; i < 100; i++) {
        total_sum += data[i];
    }

    printf("Total Sum: %d\n", total_sum);
    printf("預設執行緒數: %d\n", omp_get_max_threads());
    
#else 
    long long total_sum = 0;

    struct timespec m_start, m_end;
    clock_gettime(CLOCK_MONOTONIC, &m_start);

    // num_threads(THREADS): 強制指定使用 16 個執行緒
    // reduction(+:total_sum): 告訴編譯器幫我們做區域加總與合併
    #pragma omp parallel for num_threads(THREADS) reduction(+:total_sum)
    for (long long i = 1; i <= LIMIT; i++) {
        total_sum += i;
    }

    clock_gettime(CLOCK_MONOTONIC, &m_end);

    printf("計算完成！1 到 %lld 的總和是: %lld\n", LIMIT, total_sum);
    printf("執行時間: %f 秒\n", (m_end.tv_sec - m_start.tv_sec) + (m_end.tv_nsec - m_start.tv_nsec) / 1e9);
    printf("使用的執行緒數: %d\n", THREADS);

#endif
    return 0;
}
