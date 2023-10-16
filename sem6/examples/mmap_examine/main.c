#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("/proc/%d\n", getpid());

    int fd = open("file.dat", O_RDONLY);

    void *mem = mmap(NULL, sizeof(char) * 10, PROT_READ, MAP_SHARED, fd, 0);
    printf("mem = 0x%x\n", mem);

    void *anon = mmap(NULL, sizeof(char) * 0x2000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    printf("anon = 0x%x\n", anon);

    if (anon == (void*)-1) {
        perror("mmap anon");
        exit(2);
    }

    for (;;) {
        sleep(9999);
    }
}