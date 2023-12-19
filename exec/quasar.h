#pragma once

#include <stdint.h>
#include <stddef.h>

void syscall(uint64_t vector, void* arg1, void* arg2, void* arg3) {
    asm volatile (
        "int $0x30\n\t"
        : "+a" (vector), "+b" (arg1), "+c" (arg2), "+d" (arg3) // I/O operands
        : 
        : "memory" // probably
    );
}

void putf(const char* str, void* val) {
    syscall(1, (void*)str, val, 0);
}

void puts(const char* str) {
    syscall(0, (void*)str, 0, 0);
}

char** dir() {
    syscall(3, 0, 0, 0);
    char** ret;
    asm("":"=a"(ret)::);
    return ret;
}

int get_entries() {
    int ret;
    syscall(4, 0, 0, 0);
    asm("":"=a"(ret)::);
    return ret;
}

char* read(char* name) {
    syscall(5, name, 0, 0);
    char* ret;
    asm("":"=a"(ret)::);
    return ret;
}

uint64_t run_elf(char* name, char** args, int argc) {
    uint64_t pid;
    syscall(6, name, args, argc);
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
    syscall(7, (void*)pid, 0, 0);
    asm("":"=a"(signal)::);
    return signal;
}

uint64_t ipc_get_ret(uint64_t pid) {
    uint64_t ret;
    syscall(8, (void*)pid, 0, 0);
    asm("":"=a"(ret)::);
    return ret;
}

void ipc_dispatch(uint64_t pid) {
    syscall(9, (void*)pid, 0, 0);
}

void* kmalloc(size_t size) {
    void* addr;
    syscall(0x0a, size, 0, 0);
    asm("":"=a"(addr)::);
    return addr;
}

void kfree(void* addr) {
    syscall(0x0b, addr, 0, 0);
}