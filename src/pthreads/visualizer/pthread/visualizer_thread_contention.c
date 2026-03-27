#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 5
#define ITERATIONS 10

// --- 共享資源 ---
int shared_pot = -1;       // 所有人都要搶著修改
int use_lock = 0;          // 安全開關：0 = 沒鎖 (亂搶), 1 = 有鎖 (排隊)
pthread_mutex_t lock;

void print_ui(int id, const char* action, const char* color, int val) {
    // 每個執行緒有自己的行，顯示目前的動作和讀取到的值
    printf("\033[%d;1HThread %d: [%s%-12s\033[0m] | 讀取到的值: %2d", 
           id + 4, id, color, action, val);
    
    // 在上方顯示目前的值真相
    printf("\033[2;1H================================");
    printf("\033[3;1H [ 共享變數的現況 ] -> 【 %2d 】", shared_pot);
    printf("\033[2;35H模式: %s", use_lock ? "Mutex ON" : "No Lock");
    fflush(stdout);
}

void* thread_worker(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        // 1. 準備階段
        print_ui(id, "準備中", "\033[32m", shared_pot);
        usleep(rand() % 800000 + 200000);

        // 2. 進入爭奪
        if (use_lock) pthread_mutex_lock(&lock);
        
        print_ui(id, "正在修改!", "\033[31m", shared_pot);
        
        // 模擬「讀取 -> 修改 -> 寫回」的延遲
        int temp = shared_pot; 
        usleep(100000); // 故意製造空隙讓別人有機會插隊
        shared_pot = id; 
        
        if (use_lock) pthread_mutex_unlock(&lock);

        // 3. 修改完成
        usleep(500000);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc > 1) use_lock = atoi(argv[1]); // 可以透過指令參數控制

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);

    printf("\033[2J\033[?25l"); // 清屏、隱藏游標
    printf("\033[1;1H=== 共享變數強奪實驗 ===");

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_worker, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) pthread_join(threads[i], NULL);

    printf("\033[%d;1H\n實驗結束。請觀察落差。\033[?25h\n", NUM_THREADS + 5);
    return 0;
}