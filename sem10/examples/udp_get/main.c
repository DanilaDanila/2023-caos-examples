#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void my_assert(int mb_true, char *msg) {
    if (!mb_true) {
        perror(msg);
        exit(1);
    }
}

int main() {
  struct sockaddr_in server = {
    .sin_family = AF_INET,
    .sin_port = htons(8989),
    .sin_addr.s_addr = inet_addr("127.0.0.1"),
  };
  int server_len = sizeof(server);
  
  int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  my_assert(socket_fd > 0, "socket error");
  

  char buffer[] = "Hello, UDP!";
  my_assert(
    sendto(
      socket_fd,
      buffer,
      sizeof(buffer),
      0,
      (struct sockaddr*)(&server),
      server_len
    ),
    "sendto"
  );

  close(socket_fd);
  return 0;
}

