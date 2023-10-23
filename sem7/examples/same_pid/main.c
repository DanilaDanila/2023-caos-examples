#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *myThreadFun(void *vargp)
{
    printf("THREAD pid: %d\n", getpid());
    sleep(5);
    return NULL;
}

int main()
{
    printf("PROCESS pid: %d\n", getpid());
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, myThreadFun, NULL);
    sleep(5);
    pthread_join(thread_id, NULL);
    exit(0);
}

