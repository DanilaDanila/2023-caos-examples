#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int pipefd[2];

    if (pipe(pipefd) != 0) {
        perror("pipe");
        exit(1);
    }


    // run ls
    pid_t ls_pid = fork();

    if (ls_pid < 0) {
        perror("fork");
        exit(2);
    }

    if (ls_pid == 0) {
        close(pipefd[0]);

        close(1);
        dup2(pipefd[1], 1);

        execl("/bin/ls", "/bin/ls", "/usr/include", NULL);
        perror("exec ls");
        exit(3);
    }


    // run grep
    pid_t grep_pid = fork();

    if (grep_pid < 0) {
        perror("fork grep");
        exit(5);
    }

    if (grep_pid == 0) {
        close(pipefd[1]);

        close(0);
        dup2(pipefd[0], 0);

        execl("/bin/grep", "/bin/grep", "io", NULL);
        perror("exec grep");
        exit(4);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(ls_pid, 0, 0);
    waitpid(grep_pid, 0, 0);
}

