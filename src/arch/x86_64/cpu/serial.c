#include <arch/x86_64/cpu/serial.h>
#include <arch/x86_64/io.h>
#include <libc/string.h>
#include <libc/printf.h>
#include <sched/sched.h>

int Serial_Init() {
    outb(COM1 + 1, 0);
    outb(COM1 + 3, 0x80);
    outb(COM1, 0x03);
    outb(COM1 + 1, 0);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    outb(COM1 + 4, 0x1E);
    outb(COM1, 0xAE);

    if (inb(COM1) != 0xAE) {
        return 1;
    }

    outb(COM1 + 4, 0x0F);
    return 0;
}

int Serial_IsBusEmpty() {
    return inb(COM1 + 5) & 0x20;
}

void Serial_WriteChar(char a) {
    while (Serial_IsBusEmpty() == 0);
    outb(COM1, a);
}

void Serial_Send(char* pStr) {
    while (*pStr != '\0') {
        Serial_WriteChar(*pStr++);
    }
}

void* doNothing(void* nothing) {
    return nothing;
}

void Serial_Wrap(char c, void* extra) {
    doNothing(extra);
    Serial_WriteChar(c);
}

void Serial_Printf(char* pStr, ...) {
    Sched_Lock();
    va_list args;
    va_start(args, pStr);
    vfctprintf(Serial_Wrap, NULL, pStr, args);
    va_end(args);
    Sched_Unlock();
}