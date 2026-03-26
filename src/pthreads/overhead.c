#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define ITERATIONS 50000

void* dummy_func(void* arg) { return NULL; }

int main() {
    struct timespec m_start, m_end; // 給 Monotonic 使用
    time_t t_start, t_end;          // 給 time() 使用
    pthread_t thread;

    // 1. 記錄開始
    t_start = time(NULL); 
    clock_gettime(CLOCK_MONOTONIC, &m_start);

    for (int i = 0; i < ITERATIONS; i++) {
        pthread_create(&thread, NULL, dummy_func, NULL);
        pthread_join(thread, NULL);
    }

    // 2. 記錄結束
    clock_gettime(CLOCK_MONOTONIC, &m_end);
    t_end = time(NULL);

    // 3. 計算並顯示
    // .tv_sec 為libc定義的結構成員，表示秒數
    // .tv_nsec 為libc定義的結構成員，表示ns
    double m_diff = (m_end.tv_sec - m_start.tv_sec) + (m_end.tv_nsec - m_start.tv_nsec) / 1e9;
    double t_diff = difftime(t_end, t_start);

    printf("精確測量 (clock_gettime): %f 秒\n", m_diff);
    printf("粗略測量 (time): %f 秒\n", t_diff);
    printf("平均每個 thread 建立耗時: %f us\n", (m_diff / ITERATIONS) * 1e6);
    return 0;
}