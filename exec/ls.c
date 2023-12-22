#include <stdint.h>
#include "quasar.h"

typedef struct {
    char magic[4];
} elf_header;

int main(int argc, char** argv) {
    char** dir_ = dir();
    elf_header* exec_check;
    for (int i = 0; i < get_entries(); i++) {
        char* file = read(dir_[i]);
        exec_check = (elf_header*)file;
        if (exec_check->magic[0] == 0x7f && exec_check->magic[1] == 'E' &&
            exec_check->magic[2] == 'L' && exec_check->magic[3] == 'F') {
            term_set_fg(0xFF00FF00);
        } else {
            term_reset_fg();
        }
        putf("%s ", dir_[i]);
    }
    term_reset_fg();
    puts("\n");
    return 0;
}