#pragma once

#include <stdint.h>
#include <stdarg.h>
#include "../src/quasar.h"

void putc(char c) {
    char str[] = {c, '\0'};
    puts(str);
}

int file_read(char* path, char* buf) {
    int ret;
    syscall(5, path, buf, 0);
    asm("":"=a"(ret)::);
    return ret;
}