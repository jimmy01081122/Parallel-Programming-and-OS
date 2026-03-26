#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define LIMIT 1000000000LL // 十億
#define THREADS 10000         

// 傳給執行緒的參數包
typedef struct {
    long long start;
    long long end;
    long long partial_sum;
} ThreadData;

// 每個員工要做的工作：把分配到的範圍加起來
void* sum_range(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->partial_sum = 0;
    for (long long i = data->start; i <= data->end; i++) {
        data->partial_sum += i;
    }
    return NULL;
}

int main() {
    pthread_t threads[THREADS];
    ThreadData data[THREADS];
    long long chunk_size = LIMIT / THREADS;

    // 1. 分派任務
    for (int i = 0; i < THREADS; i++) {
        data[i].start = i * chunk_size + 1;
        data[i].end = (i == THREADS - 1) ? LIMIT : (i + 1) * chunk_size;
        pthread_create(&threads[i], NULL, sum_range, &data[i]);
    }

    // 2. 等待所有人收工，並把各組結果加起來
    long long total_sum = 0;
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_sum += data[i].partial_sum;
    }

    printf("計算完成！1 到 %lld 的總和是: %lld\n", LIMIT, total_sum);
    return 0;
}