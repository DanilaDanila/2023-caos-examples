#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int fd[2];

    if (pipe(fd) != 0) {
        perror("pipe");
        exit(2);
    }

    char buffer[256];
    char msg[] = "hello";


    if (write(fd[1], msg, sizeof(msg)) <= 0) {
        exit(1);
    }

    int len = read(fd[0], buffer, sizeof(buffer));

    write(1 /* stdout */, buffer, len);
}

