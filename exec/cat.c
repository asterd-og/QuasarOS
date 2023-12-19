#include <stdint.h>
#include <stddef.h>
#include "quasar.h"

int main(int argc, char** argv) {
    char* res = read(argv[1]);
    if (res == NULL) {
        putf("Couldn't find file '%s'.\n", argv[1]);
        return 1;
    }
    putf("%s\n", read(argv[1]));
    return 0;
}