#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main() {
    int fd[2];
    if (pipe(fd) != 0) {
        perror("pipe");
        exit(2);
    }

    int i = 0;
    char msg[] = "0123456789ABCDEF";
    while (write(fd[1], msg, sizeof(msg)) > 0) {
        ++i;
        printf("%d\n", i * sizeof(msg));
    }

    return 0;
}
