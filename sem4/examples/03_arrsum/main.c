#include <stdio.h>

extern int arrsum(int N, int *arr);

int main() {
    int N = 10;
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    int sum = arrsum(N, arr);

    printf("sum = %d\n", sum);
    return 0;
}