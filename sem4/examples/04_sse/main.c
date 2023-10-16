#include <stdio.h>

extern double do_smth(double x, double y);

int main() {
    double x = 22;
    double y = 7;

    double z = do_smth(x, y);
    printf("%lf\n", z);
}