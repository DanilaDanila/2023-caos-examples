#include <stdio.h>
#include <stdlib.h>

void call_me() {
    printf("WIN-WIN\n");
    exit(0);
}

void do_smth() {
    char name[16] = "some value";
    scanf("%s", name);
    printf("Hello, %s\n", name);
}

int main() {
    do_smth();
    return 0;
}

