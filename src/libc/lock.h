#pragma once

#include <types.h>

typedef struct {
    bool locked;
} Locker;

void Lock(Locker* lock);
void Unlock(Locker* lock);