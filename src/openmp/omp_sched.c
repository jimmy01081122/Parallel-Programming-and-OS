#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
void heavy_work(int i) {
    // 模擬耗時工作
    for (volatile long j = 0; j < (i + 1) * 10000000; j++);
}

int main() {
    int n;
    int max_threads = omp_get_max_threads();
    
    // 讓使用者自行輸入 n ，無上限，所以用heap
    printf("請輸入要測試的迭代次數 n (例如 20 或 100): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("輸入無效！請輸入正整數。\n");
        return 1;
    }

    //  Heap 配置記憶體，防止 Stack Overflow
    int *assigned_thread = (int *)malloc(n * sizeof(int));
    if (assigned_thread == NULL) {
        printf("記憶體配置失敗！n 可能太大了。\n");
        return 1;
    }

    printf("--- 測試  Scheduling (Chunk size = 1) ---\n");
    
    double start = omp_get_wtime();
    // dynamic / guided / static / auto / runtime(env var)
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        int tid = omp_get_thread_num();
        
        // 紀錄目前的 thread ID 到對應的迭代位置
        assigned_thread[i] = tid;
        
        // printf("Thread %d 執行迭代 %2d\n", tid, i); // 若迭代太多可註解掉看最後統計即可
        heavy_work(i);
    }
    
    double end = omp_get_wtime();


    printf("\n--- 分配結果統計 ---\n");
    for (int t = 0; t < max_threads; t++) {
        int count = 0;
        printf("Thread %d 處理的迭代: [ ", t);
        for (int i = 0; i < n; i++) {
            if (assigned_thread[i] == t) {
                printf("%d ", i);
                count++;
            }
        }
        printf("] (總計: %d 次)\n", count);
    }

    printf("\n執行時間: %f 秒\n", end - start);

    return 0;
}