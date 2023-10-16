#include <stdio.h>
#include <string.h>

#define MY_PASSWORD "56320"

int main() {
    char input[6];
    char password[6] = MY_PASSWORD;

    scanf("%s", input);

    if (strcmp(input, password) == 0) {
        printf("access granted!\n");
    } else {
        printf("access denied!\n");
    }

    return 0;
}
