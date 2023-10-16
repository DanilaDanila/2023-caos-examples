#include <stdio.h>
#include <stdint.h>

void type_float(float f) {
    union {
        float number;
        uint32_t bytes;
    } u;

    u.number = f;
    printf("%f\n0b", f);
    int i = 0;
    for (uint32_t mask = 1 << 31; mask != 0; mask = mask >> 1) {
        if (i == 1) {
            printf(" ");
        }
        if (i == 1 + 8) {
            printf(" ");
        }
        printf("%d", ((u.bytes & mask) > 0));
        ++i;
    }
    printf("\n\n");
}

int main() {
    type_float(5);
    type_float(6);
    type_float(0.3);
    type_float(0.25);
    type_float(1);
    type_float(-5);
}
