#include <stdint.h>
#include <stddef.h>
#include "quasar.h"
#include <stdio.h>

int main(int argc, char** argv) {
    char* buf = kmalloc(ftell(argv[1]));
    int ret = file_read(argv[1], buf);
    if (ret == 1) {
        putf("Couldn't find file '%s'.\n", argv[1]);
        kfree(buf);
        return 1;
    }
    putf("%s\n", buf);
    kfree(buf);
    return 0;
}