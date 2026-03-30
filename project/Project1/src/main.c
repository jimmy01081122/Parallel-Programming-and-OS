/*
 *  main.c
 *  version 2.0
 *  Modified ver 2.0 on: 2025/4/1
 *  Modified Content : benchmark & error detected & input thread number

 *  Created on: 2025/3/27
 *  Author: Jimmy 
*/
 
#include <time.h>
#include "utils.h"
#include <pthread.h>

// # define NUM_THREADS 4


int main (int argc, char *argv[]) {
    printf("Default\n");
    // size_t : unsigned integer type, auto-adjusted to the size of the platform (32-bit or 64-bit)
    size_t total_element = (size_t)rows * cols;
    // number of elements in the matrix = rows * cols
    printf("Total elements: %zu\n", total_element);

    // default thread = 1
    int num_threads = 1;
    if (argc > 1) {
        num_threads = atoi(argv[1]);
        if (num_threads <= 0) {
            fprintf(stderr, "執行緒數量必須大於 0\n");
            return EXIT_FAILURE;
        }
    }
    printf ("Threads to use: %d, start system\n", num_threads);
    // single precision float: 4 bytes
    // malloc return void* , float* can trasform void* to float*
    float *matrix = (float *)malloc(total_element * sizeof(float));
    // float **ptr = malloc(rows * sizeof(float *));
    // float *data = malloc(rows * cols * sizeof(float)); // 分配一整塊連續空間

    // for (int i = 0; i < rows; i++) {
    //     ptr[i] = &data[i * cols]; // 將指標指向連續空間的對應位置
    // }
    if (matrix == NULL) {
        // log error message to a file name "stderr"
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }
    // time_t time(time_t *tloc);
    // time return the current calendar time as 
    // a value of type time_t. 
    // If tloc is non-NULL, 
    // the return value is also stored 
    // in the memory pointed to by tloc.
    srand (time(NULL)); // seed the random number generator with current time
    for (size_t i = 0; i < total_element; i++) {
        // RAND_MAX is a constant defined in stdlib.h,
        // which represents the maximum value returned by the rand function
        matrix[i] = (float)rand() / RAND_MAX; // Initialize with some values, e.g., 0.0, 1.0, ..., 99.0, then repeat
    }

    printf ("Matrix initialization completed.\n");


    // ==========================================
    // Benchmark 開始：記錄起始時間
    // ==========================================
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    // ==========================================
    // In stack instead of heap, so no need to free() at the end of main()
    pthread_t threads[num_threads];
    thread_args_t thread_args[num_threads];
    thread_data_t thread_results[num_threads]; 
    // check if thread_results is correctly allocated
    float *row_max_array = malloc(MATRIX_SIZE * sizeof(float)); 
    
    // ========== heap version ==========
    // pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    // thread_args_t *thread_args = malloc(num_threads * sizeof(thread_args_t));
    // thread_data_t *thread_results = NULL;
    // posix_memalign((void **)&thread_results, 64, num_threads * sizeof(thread_data_t));






    size_t rows_per_thread = MATRIX_SIZE / num_threads;
    printf("\n %d 個thread開始作業...\n", num_threads);
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].start_row = i * rows_per_thread;
        
        // 如果是最後一個執行緒，就包辦剩下的所有行數 (避免無法整除的問題)
        thread_args[i].end_row = (i == num_threads - 1) ? MATRIX_SIZE : (i + 1) * rows_per_thread;
        thread_args[i].matrix = matrix;
        thread_args[i].thread_results = thread_results;
        thread_args[i].row_max_array = row_max_array;

        // 建立執行緒並執行 worker_task
        pthread_create(&threads[i], NULL, worker_task, &thread_args[i]);
    }

    
 
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL); 
    }
    double global_sum = 0.0;
    for (int i = 0; i < num_threads; i++) {
        global_sum += thread_results[i].local_sum;
    }
    // ==========================================
    // Benchmark 結束：記錄結束時間並計算差值
    // ==========================================
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                          (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("\n>>> 最終全局總和 (Global Sum): %f <<<\n", global_sum);
    printf(">>> 核心計算耗時: %f 秒 <<<\n", elapsed_time);

    printf("\n所有執行緒完成計算，矩陣總和: %f\n", global_sum);
    
    



    // ==========================================
    // 結果驗證階段 (Sequential Verification)
    // ==========================================
    printf("\n>>> 啟動結果驗證 (計算單執行緒基準值以供核對)... <<<\n");
    double expected_global_sum = 0.0;
    int validation_passed = 1;

    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        // 先預設該行的第一個元素是最大值
        float expected_row_max = matrix[i * MATRIX_SIZE]; 
        
        for (size_t j = 0; j < MATRIX_SIZE; j++) {
            float val = matrix[i * MATRIX_SIZE + j];
            expected_global_sum += val; // 計算單執行緒總和
            
            // 計算單執行緒極值
            if (val > expected_row_max) {
                expected_row_max = val;
            }
        }

        // 1. 驗證行極值：將單執行緒算出的極值，與多執行緒陣列中的結果比對
        if (row_max_array[i] != expected_row_max) {
            printf(" error：Row %zu 的極值不吻合！預期: %f, 實際: %f\n", 
                   i, expected_row_max, row_max_array[i]);
            validation_passed = 0;
            break; // 錯一個就不用比了，直接中斷
        }
    }

    // 2. 驗證全局總和
    // 浮點數運算會有微小的精度誤差 (Epsilon)，尤其是加法順序不同的時候。
    // 所以我們不能用 == 來比對浮點數，必須取相減的絕對值，確認誤差小於一個極小值。
    double diff = expected_global_sum - global_sum;
    if (diff < 0) diff = -diff; // 簡單的絕對值處理

    if (diff > 1e-2) { // 容許 0.01 的浮點數累加誤差
        printf("error：全局總和不吻合！預期: %f, 實際: %f\n", 
               expected_global_sum, global_sum);
        validation_passed = 0;
    }

    // 最終宣判
    if (validation_passed) {
        printf("Pass\n");
    } else {
        printf("Fail\n");
    }
    // ==========================================
    // release memory
  
    // if there's no calloc() malloc() realloc() in the code, then we don't need to free() any memory,
    // free(threads);
    // free(thread_args);
    // free(thread_results);
    // free(row_max_array); 
    free(matrix);
    printf("Matrix memory freed.\n");
    return 0;
}


