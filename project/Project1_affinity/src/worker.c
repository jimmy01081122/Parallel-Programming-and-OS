/*
 *  worker.c
 *  version 3.0
 *  Modified ver 3.0 on: 2025/4/1 
 *  Modified Content : affinity, padding
 *  Created on: 2025/3/27
 *  Author: Jimmy 
*/
// #define _GNU_SOURCE here or in utils.h (the head of the file)
#include "utils.h"



void* worker_task(void* arg) {
    // 1. transform void* arg to thread struct, pointer to the struct we defined in utils.h 
    thread_args_t *args = (thread_args_t*)arg;
    // here we can access the fields of the struct, e.g., args->thread_id, args->start_row, etc.
    int tid = args->thread_id;
    
    int core_id = args->core_id; // 從參數結構中獲取核心 ID
    
    // ==========================================
    // 執行緒綁核 (Thread Affinity) 邏輯
    // ==========================================
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);          // 1. 清空 CPU 集合
    CPU_SET(core_id, &cpuset);  // 2. 將我們指定的 core_id 加入集合
    // 取得當前執行緒並強制綁定
    pthread_t current_thread = pthread_self();
    int affinity_status = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
    if (affinity_status != 0) {
        printf("[Thread %d] 警告：無法綁定到 Core %d\n", tid, core_id);
    } else {
        // 印出來確認有綁定成功
        printf("[Thread %d] 成功綁在 CPU Core %d\n", tid, core_id);
    }
    // ==========================================

    #if USE_PADDING_WORKER
    double sum = 0.0; // 暫存用的局部變數，放在 CPU Register 裡最快！
    #else 
    args->thread_results[tid].local_sum = 0.0;
    #endif
    // 2. Start work：只跑自己的 Rows
    for (size_t i = args->start_row; i < args->end_row; i++) {
        // Columns
        float current_row_max = args->matrix[i * MATRIX_SIZE];
        for (size_t j = 0; j < MATRIX_SIZE; j++) {
            // 一維陣列模擬二維陣列的公式： 索引 = (行號 * 總列數) + 列號
            #if USE_PADDING_WORKER
            sum += args->matrix[i * MATRIX_SIZE + j];
            # else 
            args->thread_results[tid].local_sum += args->matrix[i * MATRIX_SIZE + j];
            #endif
            if (args->matrix[i * MATRIX_SIZE + j] > current_row_max) {
                current_row_max = args->matrix[i * MATRIX_SIZE + j];
            }
        }
        args->row_max_array[i] = current_row_max;
    }

    // 3. 計算完畢，把結果寫入專屬的記憶體位置 (已 Padding 過，不會有 False Sharing)
    #if USE_PADDING_WORKER
    args->thread_results[tid].local_sum = sum;
    printf("[Thread %d] 完成計算 Rows %zu ~ %zu，局部總和: %f\n", 
           tid, args->start_row, args->end_row - 1, sum);
    #else
    printf("[Thread %d] 完成計算 Rows %zu ~ %zu，局部總和: %f\n", 
           tid, args->start_row, args->end_row - 1, args->thread_results[tid].local_sum);
    #endif

    // print log to console

    

    return NULL; // 執行緒結束
}