#pragma once

#include <types.h>

#define PSF2_MAGIC0 0x72
#define PSF2_MAGIC1 0xb5
#define PSF2_MAGIC2 0x4a
#define PSF2_MAGIC3 0x86

typedef struct {
    u8  magic[4];
    u32 version;
    u32 header_size;
    u32 flags;
    u32 length;
    u32 char_size;
    u32 height, width;
} __attribute__((packed)) psf2_header;

typedef struct {
    psf2_header* header;
    u8* addr_start;
    u16 pitch;
} __attribute__((packed)) psf2_font;

psf2_font* psf2_load(char* font_addr);