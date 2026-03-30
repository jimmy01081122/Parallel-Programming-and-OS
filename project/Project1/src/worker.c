/*
 *  worker.c
 *  version 1.0
 *  Created on: 2025/3/27
 *  Author: Jimmy 
*/
 
#include "utils.h"



void* worker_task(void* arg) {
    // 1. transform void* arg to thread struct, pointer to the struct we defined in utils.h 
    thread_args_t *args = (thread_args_t*)arg;
    // here we can access the fields of the struct, e.g., args->thread_id, args->start_row, etc.
    int tid = args->thread_id;
    
    double sum = 0.0; // 暫存用的局部變數，放在 CPU Register 裡最快！

    // 2. Start work：只跑自己的 Rows
    for (size_t i = args->start_row; i < args->end_row; i++) {
        // Columns
        float current_row_max = args->matrix[i * MATRIX_SIZE];
        for (size_t j = 0; j < MATRIX_SIZE; j++) {
            // 一維陣列模擬二維陣列的公式： 索引 = (行號 * 總列數) + 列號
            sum += args->matrix[i * MATRIX_SIZE + j];
            if (args->matrix[i * MATRIX_SIZE + j] > current_row_max) {
                current_row_max = args->matrix[i * MATRIX_SIZE + j];
            }
        }
        args->row_max_array[i] = current_row_max;
    }

    // 3. 計算完畢，把結果寫入專屬的記憶體位置 (已 Padding 過，不會有 False Sharing)
    args->thread_results[tid].local_sum = sum;

    // print log to console
    printf("[Thread %d] 完成計算 Rows %zu ~ %zu，局部總和: %f\n", 
           tid, args->start_row, args->end_row - 1, sum);

    return NULL; // 執行緒結束
}