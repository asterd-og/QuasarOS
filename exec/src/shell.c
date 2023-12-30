#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "quasar.h"

int main(int argc, char** argv) {
    puts("Welcome to Quasar's shell V 0.1!\n");
    char ch = 0;
    char str[512];
    int i = 0;
    puts("> ");
    while (1) {
        syscall(0x02, 0, 0, 0);
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
    if (cmd == "") return;
    char temp[50];
    char** args = kmalloc(50 * sizeof(char));
    int ai = 0;
    int ti = 0;
    int ci = 0;
    while (1) {
        if (cmd[ci] == ' ') {
            temp[ti] = '\0';
            args[ai] = kmalloc(strlen(temp) * sizeof(char));
            strcpy(args[ai], temp);
            for (int i = 0; i < ti; i++) temp[i] = 0;
            ti = 0;
            ai++;
            ci++;
        } else if (cmd[ci] == '\0') {
            temp[ti] = '\0';
            args[ai] = kmalloc(strlen(temp) * sizeof(char));
            strcpy(args[ai], temp);
            break;
        } else {
            temp[ti] = cmd[ci];
            ti++;
            ci++;
        }
    }

    char* buf = (char*)kmalloc(ftell(args[0]));
    int ret = file_read(args[0], buf);

    if (ret != 1) {
        uint64_t pid = run_elf(args[0], args, ai + 1);
        while (ipc_get(pid) == 0) {
            // Signal 0 = nothing
            ; // halt and wait for the program to terminate
        }
        if (ipc_get(pid) == 2) {
            // SIGSEGV
            puts("Segmentation Fault!\n");
        } else {
            uint64_t ret = ipc_get_ret(pid);
            if (ret != 0)
                putf("Task returned with 0x%lx.\n", ipc_get_ret(pid));
        }
        ipc_dispatch(pid);
    } else {
        putf("No matching program for '%s'.\n", cmd);
    }
    kfree(buf);
    kfree(args);
}

void CmdHelp() {
    puts("QuasarOS Shell program. Version 0.1. Running inside an ELF file.\n\n");
}