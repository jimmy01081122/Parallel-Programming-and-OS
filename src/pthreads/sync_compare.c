#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdatomic.h>

#define NUM_ITERATIONS 10000000
#define NUM_THREADS 16

typedef struct {
    int counter;                    // 給 Mutex 與 Spinlock 使用的普通整數
    pthread_mutex_t mutex;          // OS 級別的互斥鎖
    pthread_spinlock_t spinlock;    // 使用者態的自旋鎖
    _Atomic int atomic_counter;     // 硬體級別的原子變數
} shared_data_t;

// 測試函式指標類型
typedef void* (*test_func_t)(void*);

// --- 不同的同步實作 ---
void* test_mutex(void* arg) {
    shared_data_t* data = (shared_data_t*)arg;
    for (int i = 0; i < NUM_ITERATIONS / NUM_THREADS; i++) {
        pthread_mutex_lock(&data->mutex);   // 確定鎖沒鎖
        data->counter++;                    // 臨界區 (Critical Section)
        pthread_mutex_unlock(&data->mutex); // 釋放鎖
    }
    return NULL;
}
// pthread_mutex_lock 在 Linux 下
// 通常實作為 Futex (Fast Userspace Mutex)。
// 它會先嘗試在 User-space 獲取鎖，
// 失敗後才會透過系統呼叫進入核心態掛起執行緒，
// 這是為了減少不必要的 Context Switch。

void* test_spinlock(void* arg) {
    shared_data_t* data = (shared_data_t*)arg;
    for (int i = 0; i < NUM_ITERATIONS / NUM_THREADS; i++) {
        pthread_spin_lock(&data->spinlock); // 不斷循環檢查鎖是否釋放，CPU 不休息
        data->counter++;
        pthread_spin_unlock(&data->spinlock);
    }
    return NULL;
}
// Spinlock 適合鎖定時間極短的情境。
// 但在環境中，若執行緒發生 Preemption（被 OS 強行奪走 CPU），
// 其它在 Spin 的執行緒會白白浪費整個時間片（Time Slice）。

void* test_atomic(void* arg) {
    shared_data_t* data = (shared_data_t*)arg;
    for (int i = 0; i < NUM_ITERATIONS / NUM_THREADS; i++) {
        atomic_fetch_add(&data->atomic_counter, 1);
    }
    return NULL;
}
// 對應到 x86 架構的 LOCK ADD 指令。
// 它不需要經過 OS 核心，
// 而是透過 CPU 的 Cache Coherency Protocol (如 MESI) 
// 在快取層級鎖定該記憶體行（Cache Line），
// 避免其它核心同時修改。

double run_experiment(test_func_t func, shared_data_t* data) {
    pthread_t threads[NUM_THREADS];
    struct timespec start, end;
    
    // Default 初始化計數器
    data->counter = 0;
    atomic_store(&data->atomic_counter, 0);

    // fork
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_THREADS; i++) pthread_create(&threads[i], NULL, func, data);
    for (int i = 0; i < NUM_THREADS; i++) pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);

    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    shared_data_t data;
    //pthread_mutex_init(&data.mutex, NULL);
    //pthread_spin_init(&data.spinlock, PTHREAD_PROCESS_PRIVATE);

    printf("執行緒數量: %d, 總迭代次數: %d\n", NUM_THREADS, NUM_ITERATIONS);
    //printf("Mutex    耗時: %f s\n", run_experiment(test_mutex, &data));
    //printf("Spinlock 耗時: %f s\n", run_experiment(test_spinlock, &data));
    printf("Atomic   耗時: %f s\n", run_experiment(test_atomic, &data));

    //pthread_mutex_destroy(&data.mutex);
    //pthread_spin_destroy(&data.spinlock);
    return 0;
}