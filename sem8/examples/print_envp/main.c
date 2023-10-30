#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char **envp) {
    if (*envp == NULL) {
        dprintf(2, "no any env\n");
        exit(1);
    }

    for (char **var_ptr = envp; *(var_ptr + 1) != NULL; var_ptr += 1) {
        printf("%s\n", *var_ptr);
    }

    return 0;
}
