#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h> // 必須包含此標頭檔來使用 alignas

#define ITERATIONS 100000000 
#define NUM_THREADS 4        

// 情況 A：緊鄰存放 (Bad)
// 整個陣列連續存放，4 個 long 共 32 bytes，全部擠在同一個 64-byte Cache Line
struct bad_struct {
    volatile long count[NUM_THREADS]; 
};

// 情況 B：對齊存放 (Good)
// 我們定義一個結構體，強制它在記憶體中以 64 bytes 為單位對齊
struct aligned_element {
    alignas(64) volatile long count; 
};

struct good_struct {
    struct aligned_element data[NUM_THREADS];
};

struct bad_struct s1;
struct good_struct s2;

// --- 執行緒函式與計時邏輯 (保持不變) ---

void* bench_bad(void* arg) {
    int id = *(int*)arg;
    for (long i = 0; i < ITERATIONS; i++) {
        s1.count[id]++;
    }
    return NULL;
}

void* bench_good(void* arg) {
    int id = *(int*)arg;
    for (long i = 0; i < ITERATIONS; i++) {
        s2.data[id].count++;
    }
    return NULL;
}

double get_time(void* (*func)(void*)) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, func, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);

    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    printf("--- False Sharing 效能研究 (Threads: %d) ---\n", NUM_THREADS);
    printf("Cache Line 對齊大小設定為: 64 bytes\n\n");

    double time_bad = get_time(bench_bad);
    printf("緊鄰存放 (Bad)  耗時: %.4f 秒\n", time_bad);

    double time_good = get_time(bench_good);
    printf("對齊存放 (Good) 耗時: %.4f 秒\n", time_good);

    printf("加速比 (Speedup): %.2fx\n", time_bad / time_good);
    return 0;
}