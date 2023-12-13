#pragma once

#include <types.h>

typedef struct {
    bool locked;
} Locker;

void lock(Locker* lock);
void unlock(Locker* lock);