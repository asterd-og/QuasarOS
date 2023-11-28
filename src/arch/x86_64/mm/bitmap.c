#include <arch/x86_64/mm/bitmap.h>

void Bitmap_SetBit(u8* bitmap, u64 bit) {
    bitmap[bit / 8] |= 1 << (bit % 8);
}

void Bitmap_ClearBit(u8* bitmap, u64 bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

u8 Bitmap_GetBit(u8* bitmap, u64 bit) {
    return bitmap[bit / 8] & 1 << (bit % 8);
}