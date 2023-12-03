#include <libc/lock.h>

void Lock(Locker* lock) {
    asm ("cli");
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE));
}

void Unlock(Locker* lock) {
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
    asm ("sti");
}