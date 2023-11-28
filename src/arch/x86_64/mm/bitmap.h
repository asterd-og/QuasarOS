#pragma once

#include <types.h>

void Bitmap_SetBit(u8* bitmap, u64 bit);
void Bitmap_ClearBit(u8* bitmap, u64 bit);
u8   Bitmap_GetBit(u8* bitmap, u64 bit);