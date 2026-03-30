/*
 *  utils.h
 *  version 3.0
 *  Modified ver 3.0 on: 2025/4/1 
 *  Modified Content : row max array & benchmark & error detected & input thread number
 *  Created on: 2025/3/27
 *  Author: Jimmy
 */
#ifndef UTILS_H
#define UTILS_H
#define _GNU_SOURCE
#include <stdalign.h> // 用於 alignas
#include <stdio.h>
#include <stdlib.h> 
// #include <stddef.h>
# define MATRIX_SIZE 16384
# define rows 16384
# define cols 16384
# define CACHE_LINE_SIZE 64
# define EXIT_FAILURE 1
typedef struct  {
    double local_sum;
    alignas(CACHE_LINE_SIZE) long long final_count;
} thread_data_t;
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
    size_t start_row;              // 負責計算的起始行
    size_t end_row;                // 負責計算的結束行 (不包含此行)
    float *matrix;                 // 整個矩陣的指標
    thread_data_t *thread_results; // 指向結果陣列
    float *row_max_array;          // 指向每行最大值的陣列
} thread_args_t;

// func give to threads to execute
void* worker_task(void* arg);

#endif // UTILS_H