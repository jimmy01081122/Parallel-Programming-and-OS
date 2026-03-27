#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 100000000 // 一億次運算，放大效能差距
#define NUM_THREADS 4        // 建議設定為你的實體核心數

// 情況 A：緊鄰存放 (觸發 False Sharing)
struct bad_struct {
    long volatile count[NUM_THREADS]; 
};

// 情況 B：對齊存放 (規避 False Sharing)
// 使用 64 位元組的 Padding 確保每個 count 獨立佔用一個 Cache Line
struct good_struct {
    struct {
        long volatile count;
        long padding[3]; // 1個long(8b) + 7個long(56b) = 64 bytes
    } data[NUM_THREADS];
};

struct bad_struct s1;
struct good_struct s2;

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
    
    double time_bad = get_time(bench_bad);
    printf("緊鄰存放 (Bad)  耗時: %.4f 秒\n", time_bad);

    double time_good = get_time(bench_good);
    printf("對齊存放 (Good) 耗時: %.4f 秒\n", time_good);

    printf("加速比 (Speedup): %.2fx\n", time_bad / time_good);
    return 0;
}
