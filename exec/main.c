#include <stdint.h>

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
                puts("\b \b");
            } else {
                putf("%c", ch);
                str[i] = ch;
                i++;
            }
        }
    }
    return 1;
}

void syscall(int vector, void* arg1, void* arg2) {
    asm volatile("int $0x30" : : "a"(vector), "b"(arg1), "c"(arg2));
}

void putf(const char* str, void* val) {
    syscall(1, str, val);
}

void puts(const char* str) {
    syscall(0, str, 0);
}

int strlen(const char* pStr) {
    int i = 0;
    while (*pStr != '\0') {
        i++;
        pStr++;
    }
    return i;
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
    putf("WARNING: No matching cmd for %s.\n", cmd);
}

void CmdHelp() {
    puts("QuasarOS Shell program. Version 0.1. Running inside an ELF file.\n");
}