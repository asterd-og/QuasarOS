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

int64_t ipc_get(uint64_t pid) {
    int64_t signal;
    syscall(7, (void*)pid, 0, 0);
    asm("":"=a"(signal)::);
    return signal;
}

int64_t ipc_get_ret(uint64_t pid) {
    int64_t ret;
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

void* malloc(size_t size) {
    void* addr;
    syscall(0x0c, size, 0, 0);
    asm("":"=a"(addr)::);
    return addr;
}

void free(void* addr) {
    syscall(0x0d, addr, 0, 0);
}

void term_clear() {
    syscall(0x0e, 0, 0, 0);
}

void term_set_fg(uint32_t rgb) {
    syscall(0x0f, rgb, 0, 0);
}

void term_set_bg(uint32_t rgb) {
    syscall(0x10, rgb, 0, 0);
}

void term_reset_fg() {
    syscall(0x11, 0, 0, 0);
}

void term_reset_bg() {
    syscall(0x12, 0, 0, 0);
}

size_t ftell(char* name) {
    size_t size;
    syscall(0x13, name, 0, 0);
    asm("":"=a"(size)::);
    return size;
}