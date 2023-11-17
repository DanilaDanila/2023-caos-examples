#include <unistd.h>

enum {
    buffer_size = 256,

    STDIN_FD = 0,
    STDOUT_FD = 1
};

int main(int argc, char **argv) {
    char buffer[buffer_size];

    int nlen;
    for (;;) {
        nlen = read(STDIN_FD, buffer, buffer_size);

        if (nlen <= 0) {
            break;
        }

        for (int i = 0; i < nlen; ++i) {
            if (buffer[i] == argv[1][0]) {
                write(STDOUT_FD, "1", sizeof(char));
            } else {
                write(STDOUT_FD, "0", sizeof(char));
            }
        }
    }
    return 0;
}

