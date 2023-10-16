#include <stdio.h>

extern int add2(int x, int y);

int main() {
    int x = 0x11001111;
    int y = 0x22220022;

    int z = add2(x, y);
    printf("0x%x\n", z);

    return 0;
}
