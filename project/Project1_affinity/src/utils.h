/*
 *  utils.h
 *  version 5.0
 *  Modified ver 5.0 on: 2025/4/1 
 *  Modified Content :affinity, block padding,worker variable padding
 *  Author: Jimmy
 */
#ifndef UTILS_H
#define UTILS_H
#define _GNU_SOURCE

#include <stdalign.h> // 用於 alignas
#include <stdio.h>
#include <stdlib.h> 
#include <sched.h>
#include <pthread.h>

// #include <stddef.h>
# define MATRIX_SIZE 16384
# define rows 16384
# define cols 16384
# define CACHE_LINE_SIZE 64
# define EXIT_FAILURE 1
# define USE_PADDING 0 // 1: 有防禦，0: 無防禦
# define USE_PADDING_WORKER 0 // 1: worker 裡面用局部變數 sum，0: worker 裡面直接寫入 thread_results[tid].local_sum

# if USE_PADDING
typedef struct  {
    double local_sum;
    alignas(CACHE_LINE_SIZE) long long final_count;
} thread_data_t;
#else
    typedef struct {
        volatile  double local_sum;
    } thread_data_t;
#endif
// 等價於
// typedef struct {
//     double local_sum;
//     char padding[CACHELINE_SIZE - sizeof(double)]; 
// } __attribute__((aligned(CACHELINE_SIZE))) thread_data_t;
// ---------------------------------------------------------
// typedef struct {
//     double local_sum;
//     char padding[CACHELINE_SIZE - sizeof(double)];
// } alignas(CACHELINE_SIZE) thread_data_t;

typedef struct {
    int thread_id;                 // 執行緒的編號 (0, 1, 2...)
    int core_id;                   // 執行緒綁定的核心編號
    size_t start_row;              // 負責計算的起始行
    size_t end_row;                // 負責計算的結束行 (不包含此行)
    float *matrix;                 // 整個矩陣的指標
    thread_data_t *thread_results; // 指向結果陣列
    float *row_max_array;          // 指向每行最大值的陣列
} thread_args_t;

// func give to threads to execute
void* worker_task(void* arg);

#endif // UTILS_H