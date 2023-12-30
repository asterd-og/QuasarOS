#include "quasar.h"
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        putf("Not enough arguments! Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    
    char* buf = kmalloc(ftell(argv[1]));
    size_t size = ftell(argv[1]);
    int ret = file_read(argv[1], buf);
    if (ret == 1) {
        putf("Couldn't find file '%s'!\n", argv[1]);
        kfree(buf);
        return 1;
    }

    puts("00000000: ");

    uint64_t tmp_cnt = 0;
    uint64_t i;

    for (i = 0; i < size; i++) {
        if (i > 0 && i % 8 == 0) {
            tmp_cnt = 0;
            puts("| ");
            for (uint64_t j = 8; j > 0; j--) {
                putf("%c", (buf[i - j] == 0 ? '.' : buf[i - j]));
            }
            putf("\n%08x: ", (uint64_t)i);
        }

        putf("0x%x ", (uint8_t)buf[i]);
        tmp_cnt++;
    }
    if (tmp_cnt != 0) {
        for (int i = 0; i < 8 - tmp_cnt; i++) {
            puts("     ");
        }
        puts("| ");
        for (uint64_t j = tmp_cnt; j > 0; j--) {
            putf("%c", (buf[i - j] == 0 ? '.' : buf[i - j]));
        }
    }
    puts("\n");
    kfree(buf);

    return 0;
}