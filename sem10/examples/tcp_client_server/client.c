#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

void my_assert(int mb_true, char *msg) {
    if (!mb_true) {
        perror(msg);
        exit(2);
    }
}

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    my_assert(socket_fd > 0, "socket");

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8989),
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
    };

    my_assert(
      connect(socket_fd, (struct sockaddr*)(&addr), sizeof(addr)) == 0,
      "connect"
    );

    
    char msg[] = "this is message to tcp socket";
    printf("to server: \"%s\"\n", msg);
    write(socket_fd, msg, sizeof(msg));

    char buffer[256];
    read(socket_fd, buffer, sizeof(buffer));
    printf("from server: \"%s\"\n", buffer);

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}
