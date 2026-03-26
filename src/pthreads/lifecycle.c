#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ITERATIONS 10000

// 最小任務函式
void* minimal_task(void* arg) {
    return NULL;
}

void pure_function_call() {
    minimal_task(NULL);
}

double get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main() {
    double start, end;
    pthread_t thread;

    // 1. 基準測試：普通函式呼叫 (User-space only)
    start = get_time_ns();
    for (int i = 0; i < ITERATIONS; i++) {
        pure_function_call();
    }
    end = get_time_ns();
    printf("平均純函式呼叫耗時: %.2f ns\n", (end - start) / ITERATIONS);

    // 2. 執行緒建立與銷毀 (User <-> Kernel switch)
    start = get_time_ns();
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_create(&thread, NULL, minimal_task, NULL);
        pthread_join(thread, NULL);
    }
    end = get_time_ns();
    printf("平均 pthread_create + join 耗時: %.2f ns\n", (end - start) / ITERATIONS);

    return 0;
}