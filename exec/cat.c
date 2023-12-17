#include <stdint.h>
#include <stddef.h>
#include "quasar.h"

int main() {
    char ch = 0;
    char str[100];
    int i = 0;
    puts("File name: ");
    while (1) {
        syscall(0x02, 0, 0);
        asm ("" : "=a"(ch) ::);
        if (ch != 0) {
            if (ch == '\n') {
                puts("\n");
                str[i] = '\0';
                char* res = read(str);
                if (res == NULL) {
                    return 1;
                }
                putf("%s\n", read(str));
                for (int j = 0; j < i; j++) {
                    str[j] = 0;
                }
                i = 0;
                puts("> ");
            } else if (ch == '\b') {
                if (i > 0) {
                    puts("\b \b");
                    i--;
                    str[i] = 0;
                }
            } else {
                putf("%c", ch);
                str[i] = ch;
                i++;
            }
        }
    }
    return 0;
}