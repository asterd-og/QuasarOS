#include <stdint.h>
#include <stddef.h>
#include "quasar.h"

int main() {
    puts("Welcome to Quasar's shell V 0.1!\n");
    char ch = 0;
    char str[512];
    int i = 0;
    puts("> ");
    while (1) {
        syscall(0x02, 0, 0);
        asm ("" : "=a"(ch) ::);
        if (ch != 0) {
            if (ch == '\n') {
                puts("\n");
                str[i] = '\0';
                CmdHandler(str);
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
    return 1;
}

int strcmp(const char* x, const char* y) {
    if (strlen(x) != strlen(y)) return 1;
    for (int i = 0; i < strlen(x); i++) {
        if (x[i] != y[i]) return 1;
    }
    return 0;
}


typedef void(*functionPointerType)(void);
struct commandStruct {
    char const *name;
    functionPointerType execute;
    char const *help;
};

void CmdHelp(void);

const struct commandStruct commands[] = {
    {"help", &CmdHelp, "Help menu."},
    {"",0,""}
};

void CmdHandler(char * cmd)
{
    int i = 0;
    while(commands[i].execute > 0)
    {
        if(!strcmp(commands[i].name, cmd))
        {
            (*commands[i].execute)();
            return;
        }
        i++;
    }
    if (read(cmd) != NULL) {
        uint64_t pid = run_elf(cmd);
        while (ipc_get(pid) != 0) {
            // Signal 0 = SIGKILL
            ; // halt and wait for the program to terminate
        }
        putf("Task returned with 0x%lx.\n", ipc_get_ret(pid));
    } else {
        putf("No matching program for '%s'.\n", cmd);
    }
}

void CmdHelp() {
    puts("QuasarOS Shell program. Version 0.1. Running inside an ELF file.\n\n");
}