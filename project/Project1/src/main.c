/*
 *  main.c
 *  version 1.0
 *  Created on: 2025/3/27
 *  Author: Jimmy 
*/
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include "utils.h"



int main () {
    printf("Default\n");
    // size_t : unsigned integer type, auto-adjusted to the size of the platform (32-bit or 64-bit)
    size_t total_element = (size_t)rows * cols;
    // number of elements in the matrix = rows * cols
    printf("Total elements: %zu\n", total_element);

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
    free (matrix);
 
    return 0;
}


