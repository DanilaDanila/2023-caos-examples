#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/eventfd.h>
#include <signal.h>

int pipe_fd[2];

void handler(int sig) {
    if (sig == SIGINT) {
        // graceful shutdown
        char a = 1;
        write(pipe_fd[1], &a, sizeof(a));
    }
}

void my_assert(int mb_true, char *msg) {
    if (!mb_true) {
        perror(msg);
        exit(1);
    }
}

int main() {
    struct sigaction sa = {
        .sa_handler = handler,
        .sa_flags = SA_RESTART,
    };
    sigemptyset(&sa.sa_mask);

    my_assert(
        sigaction(SIGINT, &sa, NULL) == 0,
        "sigaction"
    );

    int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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
    printf("binded!\n");

    my_assert(
      listen(socket_fd, SOMAXCONN) == 0,
      "listen"
    );

    int epoll_fd = epoll_create1(0);
    my_assert(epoll_fd > 0, "epoll");

    /****************************************************/

    // добавляем socket_fd в epoll чтоб ждать клиентов
    {
        struct epoll_event event = {
            .events = EPOLLIN,
            .data.fd = socket_fd,
        };

        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);
    }

    /****************************************************/

    // добавляем pipe_fd для graceful shutdown
    // (на самом деле нифига; и это надо обсудить)
    {
        pipe(pipe_fd);
        fcntl(pipe_fd[0], F_SETFD, (fcntl(pipe_fd[0], F_GETFD) | O_NONBLOCK));


        struct epoll_event event = {
            .events = EPOLLIN,
            .data.fd = pipe_fd[0],
        };

        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fd[0], &event);
    }

    /****************************************************/

    printf("Server started...\n");
    for (;;) {
        struct epoll_event event;
        int N = epoll_wait(epoll_fd, &event, 1, 1000);
        if (N <= 0) {
            continue;
        }

        if (event.data.fd == pipe_fd[0]) {

            // УРА!
            close(epoll_fd);
            break;

        } else if (event.data.fd == socket_fd) {

            int client_fd = accept(socket_fd, NULL, NULL);
            fcntl(client_fd, F_SETFD, (fcntl(client_fd, F_GETFD) | O_NONBLOCK));

            // шедулим только отправку (оно идет первым шагом)
            struct epoll_event client_event = {
                .events = EPOLLOUT,
                .data.fd = client_fd,
            };

            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);

        } else if (event.events == EPOLLOUT) /* клиент готов что-то сказать */ {

            int client_fd = event.data.fd;

            char msg[] = "hello, client! I am `epoll`";
            write(client_fd, msg, sizeof(msg));

            // больше не интересуемся, готов ли клиент что-то сказать
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);

            // шедулим готовность чтения (второй шаг)
            struct epoll_event client_event = {
                .events = EPOLLIN,
                .data.fd = client_fd,
            };

            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);

       } else if (event.events == EPOLLIN) /* клиент хочет что-то сказать */ {

            int client_fd = event.data.fd;

            char buffer[256];
            read(client_fd, buffer, sizeof(buffer));

            shutdown(client_fd, SHUT_RDWR);
            close(client_fd);
        }
   }
    
    // закрываем сокет
    close(socket_fd);

    return 0;
}

