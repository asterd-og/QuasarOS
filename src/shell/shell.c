#include <shell/shell.h>
#include <libc/string.h>
#include <drivers/kb.h>

void CmdHandler(char * cmd);

void Shell_Init() {
    printf("Welcome to Quasar's shell V 0.1!\n");
    char ch = 0;
    char str[512];
    int i = 0;
    printf("> ");
    while (1) {
        ch = KB_GetChar();
        if (ch != 0) {
            if (ch == '\n') {
                printf("\n");
                str[i] = '\0';
                CmdHandler(str);
                for (int j = 0; j < i; j++) {
                    str[j] = 0;
                }
                i = 0;
                printf("> ");
            } else if (ch == '\b') {
                printf("\b \b");
            } else {
                printf("%c", ch);
                str[i] = ch;
                i++;
            }
        }
    }
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
    {"ls", &CmdLs, "List dir."},
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
    char* ELF_Addr = QuasFS_Read(cmd);
    if (ELF_Addr == NULL) {
        printf("Couldn't open file!\n");
        return;
    }
    Sched_CreateNewElf(ELF_Addr);
}

void CmdHelp() {
    printf("QuasarOS Shell program. Version 0.1.\n");
    int i = 0;
    while(commands[i].execute > 0)
    {
        printf("%s - ", commands[i].name);
        printf("%s\n", commands[i].help);
        i++;
    }
}

void CmdLs() {
    char** dir = QuasFS_Dir();
    int entryCount = QuasFS_GetEntryCount();
    for (int i = 0; i < entryCount; i++) {
        printf("%s ", dir[i]);
    }
    printf("\n");
}