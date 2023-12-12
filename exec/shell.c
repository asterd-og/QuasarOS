#include <stdint.h>
#include <stddef.h>

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
    while(1);
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

char** dir() {
    syscall(3, 0, 0);
    char** ret;
    asm("":"=a"(ret)::);
    return ret;
}

int getEntries() {
    int ret;
    syscall(4, 0, 0);
    asm("":"=a"(ret)::);
    return ret;
}

char* read(char* name) {
    syscall(5, name, 0);
    char* ret;
    asm("":"=a"(ret)::);
    return ret;
}

void newElf(char* name) {
    syscall(6, name, 0);
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
void CmdLs(void);

const struct commandStruct commands[] = {
    {"help", &CmdHelp, "Help menu."},
    {"ls", &CmdLs, "Lists the directory."},
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
    if (read(cmd) == NULL) {
        putf("No matching program for '%s'\n", cmd);
    }
    newElf(cmd);
    while(1);
}

void CmdLs() {
    char** dir_ = dir();
    for (int i = 0; i < getEntries(); i++) {
        putf("%s ", dir_[i]);
    }
    puts("\n");
}

void CmdHelp() {
    puts("QuasarOS Shell program. Version 0.1. Running inside an ELF file.\n");
    int i = 0;
    while(commands[i].execute > 0)
    {
        putf("%s - ", commands[i].name);
        putf("%s\n", commands[i].help);
        i++;
    }
}