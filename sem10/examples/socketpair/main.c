#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

int main() {
  int fd[2];
  socketpair(AF_UNIX, SOCK_STREAM, 0, fd);


  // 0 <---> 1
  // write to fd[0] --> read from fd[1]
  // !WRONG! write to fd[0] --> read from fd[0]

  // write to fd[0]; read from fd[1]
  {
    char msg[] = "write to fd[0]\n";
    write(fd[0], msg, sizeof(msg));

    char buffer[1024];
    size_t len = read(fd[1], buffer, sizeof(buffer));
    write(1, buffer, len);
  }

  // vice versa
  {
    char msg[] = "and vice versa!\n";
    write(fd[1], msg, sizeof(msg));

    char buffer[1024];
    size_t len = read(fd[0], buffer, sizeof(buffer));
    write(1, buffer, len);
  }

  close(fd[0]);
  close(fd[1]);
  return 0;
}

