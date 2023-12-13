#pragma once

#include <types.h>

void bitmap_set(u8* bitmap, u64 bit);
void bitmap_clear(u8* bitmap, u64 bit);
u8   bitmap_get(u8* bitmap, u64 bit);