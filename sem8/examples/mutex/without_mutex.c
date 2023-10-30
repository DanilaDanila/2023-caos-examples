#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int global_int;

void *worker(void *arg) {
    int *gi = (int*)arg;
    
    for (int i = 0; i < 1000; ++i) {
        // (*gi)++;
        int old = *gi;
        int new = old + 1;
        sched_yield();
        *gi = new;
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
    
    return 0;
}

