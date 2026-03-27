#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <stdbool.h>
#include <stdalign.h>

#define CORE 16

// -O3，編譯器會進行「暫存器優化」。
// 在這個執行緒的視角裡，沒有人會動這個變數，
// 所以不需要每次都去記憶體讀取，
// 直接把它放在 CPU 暫存器裡跑就好。
// 結果就是：就算主程式把 keep_running 改成 false 了，
// 子執行緒還是在看自己暫存器裡的舊值（true），永遠停不下來。 
// 加上 volatile 是強制 CPU 每次都要回記憶體看最新的值。
volatile bool keep_running = true;

// 儲存每個執行緒的結果
typedef struct {
    alignas(64) unsigned long long final_count;
    int thread_id;
    int bound_core;
} ThreadResult;

void* heavy_worker(void* arg) {
    ThreadResult* result = (ThreadResult*)arg;
    int core_to_bind = result->bound_core;

    // 1. 設定執行緒親和性 (Affinity)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_to_bind, &cpuset);

    pthread_t current_thread = pthread_self();
    if (pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("pthread_setaffinity_np");
        return NULL;
    }

    // 2. 密集運算迴圈
    unsigned long long local_count = 0;
    while (keep_running) {
        local_count++; // 單純的加法運算
    }

    result->final_count = local_count;
    return NULL;
}


int main() {
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("=== P-core vs E-core 算力競賽 ===\n");
    printf("系統偵測到核心數: %d\n", num_cores);
    printf("將啟動 %d 個執行緒，分別綁定到核心 0-%d\n", CORE, CORE-1);
    printf("...\n\n");

    pthread_t threads[CORE];
    ThreadResult results[CORE];

    // 3. 啟動執行緒，每個執行緒綁定到對應編號的核心
    for (int i = 0; i < CORE; i++) {
        results[i].thread_id = i;
        results[i].bound_core = i; // 這裡你可以手動更改，例如只測 0 和 12
        results[i].final_count = 0;
        pthread_create(&threads[i], NULL, heavy_worker, &results[i]);
    }

    // 4. 固定測試時間
    printf("正在運算中，請查看 htop...\n");
    fflush(stdout);
    
    getchar(); // 等待使用者按下 Enter 鍵來結束測試
    keep_running = false; // 停止所有執行緒

    // 5. 等待回收並打印結果
    for (int i = 0; i < CORE; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n--- 實驗結果 (5秒總算力) ---\n");
    printf("%-10s | %-10s | %-20s\n", "Thread ID", "Core ID", "Total Increments");
    printf("--------------------------------------------------\n");
    
    for (int i = 0; i < CORE; i++) {
        printf("Thread %-2d | Core %-2d   | %-20llu\n", 
               results[i].thread_id, results[i].bound_core, results[i].final_count);
    }

    return 0;
}