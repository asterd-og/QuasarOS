#include <stdint.h>
#include "quasar.h"

int main() {
    char** dir_ = dir();
    for (int i = 0; i < get_entries(); i++) {
        putf("%s ", dir_[i]);
    }
    puts("\n");
    return 0;
}