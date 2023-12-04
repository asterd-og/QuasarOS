#pragma once

#include <types.h>
#include <limine.h>
#include <heap/heap.h>

typedef struct {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
    u16 pitch;
    u32* buffer;
} Framebuffer;

Framebuffer* FB_CreateNewFB(u32 x, u32 y, u32 width, u32 height, u32 pitch);

void FB_SetPix(Framebuffer* fb, u32 x, u32 y, u32 argb);
u32 FB_GetPix(Framebuffer* fb, u32 x, u32 y);

void FB_CopyFB(Framebuffer* from, Framebuffer* to);

void FB_Clear(Framebuffer* fb, u32 argb);
