// omp_trace.c
#include <omp.h>
#include <stdio.h>
#include <unistd.h>

void trace_info(const char* stage) {
    int id = omp_get_thread_num();
    int total = omp_get_num_threads();
    // 使用 printf 觀察執行緒的「分身」與「匯合」
    printf("[Thread %d/%d] Stage: %s\n", id, total, stage);
}

int main() {
    printf("--- 進入主執行緒 (Master Only) ---\n");
    trace_info("Sequential Start");

    printf("\n--- 觸發 #pragma omp parallel (Forking...) ---\n");
    #pragma omp parallel
    {
        // 這裡會印出多行，代表 Fork 成功
        trace_info("Inside Parallel Region");
        
        #pragma omp master
        {
            printf("  > 只有 Master 執行緒 (%d) 會跑這行\n", omp_get_thread_num());
        }
    }

    printf("\n--- 離開並行區域 (Joined) ---\n");
    trace_info("Sequential End");

    return 0;
}