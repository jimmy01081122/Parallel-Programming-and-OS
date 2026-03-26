#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h> // 為了呼叫 SYS_gettid
#include <sys/types.h>

void* thread_function(void* arg) {
    // 將傳進來的通用指標(void*) 轉回整數指標並取值
    int id = *(int*)arg;
    // getpid(): 獲取進程 ID
    // syscall(SYS_gettid): 獲取 Linux 核心分配給這個執行緒的真正 ID
    printf("執行緒 %d: Process ID (PID) = %d, Thread ID (TID) = %ld\n", 
            id, getpid(), syscall(SYS_gettid));
    
    sleep(10); // 暫停一下，方便我們在終端機觀察
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    printf("主執行緒: Process ID (PID) = %d, Thread ID (TID) = %ld\n", 
            getpid(), syscall(SYS_gettid));
    
    
    pthread_create(&t1, NULL, thread_function, &id1);
    pthread_create(&t2, NULL, thread_function, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
// 為什麼要用 void*？ 平行程式設計中，
// pthread_create 規定任務函式必須長這樣。
// 你可以傳入任何資料的位址。

// PID vs TID： 
// getpid() 一樣       # task_struct.tgid (thread group ID) 
// gettid() 不相同     #True process ID
// 它們共用同一個 Process 空間
// 但各自擁有獨立的核心調度實體。