#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

enum {
    N_ITERATIONS = 10000000,
};


void l0_do_nothing() {
    for (register int i = 0; i < N_ITERATIONS; ++i) {
        __asm__("nop");
    }

   //  // you laugh you lose
   //  for (register int i = 0; i < N_ITERATIONS; ++i) {
   //      __asm__("nop");
   //  }
}


void l1_do_nothing() {
    for (volatile int i = 0; i < N_ITERATIONS; ++i) {
        __asm__("nop");
    }
}


void* l2_do_nothing(void *arg) {
    while (*((int*)arg) == 0) {
        // ждем, пока запустится таймер
    }

    for (; (*((int*)arg))++ < N_ITERATIONS; ) {
        __asm__("nop");
    }

    // сигнализируем, что таймер можно останавливать
    *((int*)arg) = 0;

    return NULL;
}


void mmap_do_nothing(int *mmaped_value) {
    for (*mmaped_value = 0; *mmaped_value < N_ITERATIONS; ++*mmaped_value) {
        __asm__("nop");
        msync(mmaped_value, sizeof(int), MS_SYNC);
    }
}


int main() {
    // +-heat up
    {
        for (int i = 0; i < N_ITERATIONS / 100; ++i) {
            int my_super_arr[200];
            for (int j = 0; j < 200; ++j) {
                my_super_arr[j] = j;
                __asm__("nop");
            }
        }
    }


    // L0 (register)
    {
        clock_t time_delta = clock();
        l0_do_nothing();
        time_delta = clock() - time_delta;
        
        printf("l0        (I hope): %lf\n", (double)time_delta / CLOCKS_PER_SEC);
    }


    // L1 cache
    {
        clock_t time_delta = clock();
        l1_do_nothing();
        time_delta = clock() - time_delta;
        
        printf("l1        (I hope): %lf\n", (double)time_delta / CLOCKS_PER_SEC);
    }


    // L2 cache
    {
        volatile int shared = 0;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, l2_do_nothing, (void*)&shared);

        clock_t time_delta = clock();
        shared = 1; // вот тут стартует второй поток

        while (shared != 0) {
            // а тут мы его дожидаемся
        }
        time_delta = clock() - time_delta;

        pthread_join(thread_id, NULL);
        
        printf("l2        (I hope): %lf\n", (double)time_delta / CLOCKS_PER_SEC);
    }


    // mmap with COW
    {
        // int fd = open("somefile.dat", O_RDONLY);
        int fd = open("/tmp/delete_me/abc.dat", O_RDONLY);
        int *filedata = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        close(fd);

        clock_t time_delta = clock();
        mmap_do_nothing(filedata);
        time_delta = clock() - time_delta;

        munmap(filedata, sizeof(int));
        
        printf("mmap&COW  (I hope): %lf\n", (double)time_delta / CLOCKS_PER_SEC);
    }


    // true mmap
    {
        // int fd = open("somefile.dat", O_RDWR);
        int fd = open("/tmp/delete_me/abc.dat", O_RDWR);
        int *filedata = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);

        clock_t time_delta = clock();
        mmap_do_nothing(filedata);
        time_delta = clock() - time_delta;

        munmap(filedata, sizeof(int));
        
        printf("true mmap (I hope): %lf\n", (double)time_delta / CLOCKS_PER_SEC);
    }

    return 0;
}
