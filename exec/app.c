#include <stdint.h>
#include <stddef.h>

int main() {
    puts("ELF LOADING! Made by Astrido, in QuasarOS ;)\n");
    while (1) {
        asm ("hlt");
    }
    return 0;
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