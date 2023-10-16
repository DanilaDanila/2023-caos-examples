#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int fd = open("file.dat", O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }

    char *buf = (char*)mmap(NULL, sizeof(char) * 8, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buf == MAP_FAILED) {
        perror("mmap failed");
        exit(2);
    }
    close(fd);

    write(1, buf, sizeof(char) * 8);

    munmap(buf, sizeof(char) * 8);

    return 0;
}
