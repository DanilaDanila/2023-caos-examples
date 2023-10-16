#define FUSE_USE_VERSION 30
#include <fuse.h>

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

/* чтоьб компилятор не ругался на `unused variable` нужно как-то её использовать
 * поэтому кастим к void
 */
#define UNUSED(x) ((void)x)

/* /                <-- fake root
 * └── white_rabbit <-- fake regular file
 */

const char white_rabbit_caption[] = "follow me\n";

size_t min(size_t a, size_t b)
{
    return (a < b) ? a : b;
}

int my_readdir(const char *path, void *out, fuse_fill_dir_t filler, off_t off,
               struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    UNUSED(off);
    UNUSED(fi);
    UNUSED(flags);

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    filler(out, "white_rabbit", NULL, 0, 0);

    return 0;
}

int my_getattr(const char* path, struct stat* stbuf, struct fuse_file_info* fi)
{
    UNUSED(fi);

    if (strcmp(path, "/") == 0)
    {
        *stbuf = (struct stat){.st_nlink = 1, .st_mode = S_IFDIR | 0555};
        return 0;
    }
    
    if (strcmp(path, "/white_rabbit") == 0) {
        *stbuf = (struct stat){
            .st_nlink = 1,
            .st_size = sizeof(white_rabbit_caption),
            .st_mode = S_IFREG | 0444,
        };
        return 0;
    }

    return -ENOENT;
}

int my_read(const char* path, char* buf, size_t size,
            off_t offset, struct fuse_file_info* fi)
{
    if (strcmp(path, "/") == 0)
        return -EISDIR;

    if (strcmp(path, "/white_rabbit") == 0)
    {
        /* нас могут попросить читать файл не с его начала
         * (например, если файл большой)
         * поэтому нам нужно аккуратно следить, чтоб не выйти за границу массива
         */
        size = (offset > sizeof(white_rabbit_caption)) ? 0 : min(sizeof(white_rabbit_caption) - offset, size);
        memcpy(buf, rabbit + offset, size);

        return size;
    }

    return -EIO;
}

/* специальная структурка с функциями для конкретно вашей
 * реализации файловой системы
 */
static struct fuse_operations operations = {
    .readdir = my_readdir,
    .getattr = my_getattr,
    .read = my_read,
};

int main(int argc, char *argv[]) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    int ret = fuse_main(
            args.argc,
            args.argv,
            &operations,
            NULL
    );

    fuse_opt_free_args(&args);
    return ret;
}

