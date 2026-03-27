/*
 *  utils.h
 *  version 1.0
 *  Created on: 2025/3/27
 *  Author: Jimmy
 */
#ifndef UTILS_H
#define UTILS_H
#define _GNU_SOURCE
#include <stdalign.h> // 用於 alignas
# define MATRIX_SIZE 16384
# define rows 16384
# define cols 16384
# define CACHE_LINE_SIZE 64
# define EXIT_FAILURE 1
typedef struct  {
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


#endif // UTILS_H