#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

enum {
    BUFFER_SIZE = 512,
};

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage:\n  %s /path/to/file\n", argv[0]);
        return 1;
    }


    /* 0 -- stdin
     * 1 -- stdout
     * 2 -- stderr
     */
    int fd = open(argv[1], O_RDONLY);

    if (fd == -1) {
        perror("open failed");
        return 2;
    }


    char buffer[BUFFER_SIZE];
    ssize_t bytes_count;

    for (;;) {
        bytes_count = read(fd, buffer, BUFFER_SIZE);

        if (bytes_count == -1) {
            perror("read failed");
            return 4;
        }

        if (bytes_count == 0) {
            break;
        }

        ssize_t write_offset = 0;
        char *pbuffer = &buffer[0];
        while (write_offset < bytes_count) {
            pbuffer += write_offset;
            bytes_count -= write_offset;

            write_offset = write(
                    1,
                    pbuffer,
                    bytes_count
            );
        }
    }


    if (close(fd) == -1) {
        perror("close failed");
        return 3;
    }

    return 0;
}

