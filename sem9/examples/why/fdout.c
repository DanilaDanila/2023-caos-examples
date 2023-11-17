#include <unistd.h>

int main() {
    char msg[] = "this is message to 5 fd\n";
    if (write(5, msg, sizeof(msg)) <= 0) {
        exit(1);
    }
    
    write(2, "error!\n", sizeof("error!\n"));
}
