#include <stdint.h>

void syscall(int vector, void* arg1, void* arg2);
void putf(const char* str, void* val);
void puts(const char* str);

_start() {
    char msg[] = "Tasking!";
    while(1) {
        puts(msg);
    }
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