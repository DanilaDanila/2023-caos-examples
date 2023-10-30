#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// uint8_t mutex = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int global_int;

void *worker(void *arg) {
    int *gi = (int*)arg;
    
    for (int i = 0; i < 1000; ++i) {
        pthread_mutex_lock(&mutex);
        
        int old = *gi;
        int new = old + 1;
        sched_yield();
        *gi = new;
        
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    global_int = 0;
    pthread_t threads[2];
    
    pthread_create(threads + 0, NULL, worker, (void*)(&global_int));
    pthread_create(threads + 1, NULL, worker, (void*)(&global_int));
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
    printf("%d\n", global_int);
    

    pthread_mutex_destroy(&mutex);
    return 0;
}

