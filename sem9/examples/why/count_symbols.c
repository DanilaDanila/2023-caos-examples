#include <unistd.h>
#include <stdio.h>

enum {
    buffer_size = 256,

    STDIN_FD = 0,
    STDOUT_FD = 1
};

int main(int argc, char **argv) {
    char buffer[buffer_size];

    int total = 0;
    int nlen;
    for (;;) {
        nlen = read(STDIN_FD, buffer, buffer_size);

        if (nlen <= 0) {
            break;
        }

        for (int i = 0; i < nlen; ++i) {
            if (buffer[i] == argv[1][0]) {
                ++total;
            } else {
                // nop
            }
        }
    }
    printf("%d\n", total);
    return 0;
}

