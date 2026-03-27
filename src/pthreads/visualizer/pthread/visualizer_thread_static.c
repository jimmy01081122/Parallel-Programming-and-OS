#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 4
#define TOTAL_STEPS 10

pthread_mutex_t lock;
int lock_counts[NUM_THREADS] = {0}; // 紀錄每個 Thread 搶到鎖的次數

void print_status(int id, const char* status, const char* color) {
    // 顯示 ID, 狀態, 以及搶到鎖的累積次數
    printf("\033[%d;1HThread %d: [%s%-12s\033[0m] | Success: %d 次", 
           id + 3, id, color, status, lock_counts[id]);
    fflush(stdout);
}

void* thread_worker(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < TOTAL_STEPS; i++) {
        print_status(id, "THINKING", "\033[32m"); // 綠色
        usleep(rand() % 500000); 

        print_status(id, "WAITING...", "\033[33m"); // 黃色
        pthread_mutex_lock(&lock);
        
        // --- 進入臨界區 ---
        lock_counts[id]++; 
        print_status(id, "LOCKED!!", "\033[31m"); // 紅色
        usleep(300000); 
        // ----------------
        
        pthread_mutex_unlock(&lock);
    }
    print_status(id, "FINISHED", "\033[34m");
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);
    srand(time(NULL));

    printf("\033[2J\033[?25l\033[1;1H=== 資源爭奪計數器 ===\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_worker, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) pthread_join(threads[i], NULL);
    printf("\033[%d;1H\n總結：即便核心再多，鎖（Lock）依然讓執行變回了排隊。\n\033[?25h", NUM_THREADS + 4);
    pthread_mutex_destroy(&lock);
    return 0;
}