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
    my_assert(socket_fd, "socket");

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8989),
        .sin_addr.s_addr = INADDR_ANY,
    };

    my_assert(
      bind(socket_fd, (struct sockaddr*)(&addr), sizeof(addr)) == 0,
      "bind"
    );
    my_assert(
      listen(socket_fd, SOMAXCONN) == 0,
      "listen"
    );


    int client_fd = accept(socket_fd, NULL, NULL);
    my_assert(client_fd > 0, "accept");

    char buffer[256];
    read(client_fd, buffer, sizeof(buffer));
    printf("from client: \"%s\"\n", buffer);

    char msg[] = "message to client";
    printf("to client: \"%s\"\n", msg);
    write(client_fd, msg, sizeof(msg));

    // заканчиваем взаимодействие с клиентом
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);

    // закрываем сокет
    close(socket_fd);

    return 0;
}

