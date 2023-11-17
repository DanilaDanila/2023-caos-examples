#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void my_assert(int mb_true, char *msg) {
    if (!mb_true) {
        perror(msg);
        exit(1);
    }
}

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    my_assert(socket_fd > 0, "socket");

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(8989),
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
    };

    my_assert(
        connect(socket_fd, (struct sockaddr*)(&server), sizeof(server)) == 0,
        "connect"
    );

    /****************************************************/

    char buffer[512];
    read(socket_fd, buffer, sizeof(buffer));

    // ATTENTION!
    sleep(1);

    char msg[] = "hello, server!";
    write(socket_fd, msg, sizeof(msg));

   /****************************************************/

    close(socket_fd);

    printf("%d: read(\"%s\") --> write(\"%s\")\n", getpid(), buffer, msg);

    return 0;
}
