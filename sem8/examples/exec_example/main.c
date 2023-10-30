#include <unistd.h>

int main() {
    execl(
    /* pathname */    "/usr/bin/ls",
    /* arg_0 */       "aaaaaaaaaaaaaaaaaaaaaaaa",
    /* arg_1 */       "/foo_bar/",
    /* terminator */  NULL
    );
    return 1;
}
