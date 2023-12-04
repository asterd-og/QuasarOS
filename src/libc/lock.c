#include <libc/lock.h>
#include <arch/x86_64/tables/idt/idt.h>

void Lock(Locker* lock) {
    if (lock->locked) return;
    asm ("cli");
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE));
}

void Unlock(Locker* lock) {
    if (!lock->locked) return;
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
    asm ("sti");
}