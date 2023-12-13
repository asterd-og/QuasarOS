#include <libc/lock.h>
#include <arch/x86_64/tables/idt/idt.h>

void lock(Locker* lock) {
    if (lock->locked) return;
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE));
}

void unlock(Locker* lock) {
    if (!lock->locked) return;
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}