#pragma once

#include <stdint.h>

void syscall(uint64_t vector, void* arg1, void* arg2) {
    asm volatile (
        "int $0x30\n\t"
        : "+a" (vector), "+b" (arg1), "+c" (arg2) // I/O operands
        : 
        : "memory" // probably
    );
}

void putf(const char* str, void* val) {
    syscall(1, (void*)str, val);
}

void puts(const char* str) {
    syscall(0, (void*)str, 0);
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

uint64_t run_elf(char* name) {
    uint64_t pid;
    syscall(6, name, 0);
    asm("":"=a"(pid)::);
    return pid;
}

int strlen(const char* pStr) {
    int i = 0;
    while (*pStr != '\0') {
        i++;
        pStr++;
    }
    return i;
}

uint64_t ipc_get(uint64_t pid) {
    uint64_t signal;
    syscall(7, (void*)pid, 0);
    asm("":"=a"(signal)::);
    return signal;
}

uint64_t ipc_get_ret(uint64_t pid) {
    uint64_t ret;
    syscall(8, (void*)pid, 0);
    asm("":"=a"(ret)::);
    return ret;
}