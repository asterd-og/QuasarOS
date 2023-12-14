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

int get_entries() {
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

int sched_get_tid() {
    int ret;
    syscall(7, 0, 0);
    asm("":"=a"(ret)::);
}

uint64_t sched_get_usage(int tid) {
    uint64_t ret;
    syscall(8, tid, 0);
    asm("":"=a"(ret)::);
}

char* sched_get_name(int tid) {
    char* ret;
    syscall(9, tid, 0);
    asm("":"=a"(ret)::);
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
void CmdTop(void);

const struct commandStruct commands[] = {
    {"help", &CmdHelp, "Help menu."},
    {"ls", &CmdLs, "Lists the directory."},
    {"top", &CmdTop, "Lists the running tasks."},
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
    putf("No matching program for '%s'.\n", cmd);
}

void CmdLs() {
    char** dir_ = dir();
    for (int i = 0; i < get_entries(); i++) {
        putf("%s ", dir_[i]);
    }
    puts("\n");
}

void CmdHelp() {
    puts("QuasarOS Shell program. Version 0.1. Running inside an ELF file.\n\n");
    int i = 0;
    while(commands[i].execute > 0)
    {
        putf("%s - ", commands[i].name);
        putf("%s\n", commands[i].help);
        i++;
    }
}

void CmdTop() {
    for (int i = 0; i < sched_get_tid(); i++) {
        putf("Task %d ", i);
        putf("'%s' - Usage: ", sched_get_name(i));
        putf("%ld%%\n", sched_get_usage(i));
    }
}