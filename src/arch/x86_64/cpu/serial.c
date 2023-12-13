#include <arch/x86_64/cpu/serial.h>
#include <arch/x86_64/io.h>
#include <libc/string.h>
#include <libc/printf.h>
#include <sched/sched.h>

int serial_init() {
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

int serial_is_bus_empty() {
    return inb(COM1 + 5) & 0x20;
}

void serial_write_char(char a) {
    while (serial_is_bus_empty() == 0);
    outb(COM1, a);
}

void Serial_Send(char* pStr) {
    while (*pStr != '\0') {
        serial_write_char(*pStr++);
    }
}

void serial_wrap(char c, void* extra) {
    (void)extra;
    serial_write_char(c);
}

void serial_printf(char* pStr, ...) {
    sched_lock();
    va_list args;
    va_start(args, pStr);
    vfctprintf(serial_wrap, NULL, pStr, args);
    va_end(args);
    sched_unlock();
}