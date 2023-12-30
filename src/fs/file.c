#include <fs/file.h>
#include <libc/string.h>

int file_read(char* path, char* buf) {
    char* file = quasfs_read(path);
    if (file == NULL) {
        return 1;
    }
    memcpy(buf, quasfs_read(path), quasfs_ftell(path));
    return 0;
}