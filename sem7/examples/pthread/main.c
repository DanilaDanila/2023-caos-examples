#include <stdio.h>
#include <pthread.h>

void *do_smth(void *arg) {
    int *num = (int*)arg;
    *num = -123;
    return NULL;
}

int main() {
    pthread_t child;

    int num = 123;
    printf("num = %d\n", num);

    int ret = pthread_create(
            &child,
            NULL,
            do_smth,
            &num
    );


    if (ret != 0) {
        return 2;
    }


    int j = pthread_join(child, NULL);

    printf("num = %d\n", num);
}
