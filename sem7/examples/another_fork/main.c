#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void i_am_first_fork() {
    dprintf(2, "first");
    // fflush(stdout);

    sleep(1);
    _exit(123);
}

void i_am_second_fork() {
    dprintf(2, "second");
    // fflush(stdout);

    sleep(1);
    _exit(-1);
}

int main() {
    pid_t pid1 = fork();
    if (pid1 < 0) {
        _exit(2);
    }

    if (pid1 == 0) {
        i_am_first_fork();
    }


    pid_t pid2 = fork();
    if (pid2 < 0) {
        _exit(2);
    }

    if (pid2 == 0) {
        i_am_second_fork();
    }

    int wstatus1;
    waitpid(pid1, &wstatus1, 0);

    int wstatus2;
    waitpid(pid2, &wstatus2, 0);

    printf("\n---\n");
    printf("wstatus1=%d\n", WEXITSTATUS(wstatus1));
    printf("wstatus2=%d\n", WEXITSTATUS(wstatus2));
    return 0;
}

