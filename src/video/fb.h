#pragma once

#include <types.h>
#include <limine.h>
#include <heap/heap.h>
#include <video/psf2.h>

typedef struct {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
    u16 pitch;
    u32* buffer;
} framebuffer;

framebuffer* fb_create_new(u32 x, u32 y, u32 width, u32 height, u32 pitch);

void fb_set_pix(framebuffer* fb, u32 x, u32 y, u32 argb);
u32 fb_get_pix(framebuffer* fb, u32 x, u32 y);

void fb_copy(framebuffer* from, framebuffer* to);

void fb_fill_rect(framebuffer* fb, u32 x, u32 y, u32 width, u32 height, u32 argb);

void fb_draw_char(framebuffer* fb, psf2_font* font, char ch, u32 x, u32 y, u32 argb);
void fb_draw_string(framebuffer* fb, psf2_font* font, char* str, u32 x, u32 y, u32 argb);

void fb_clear(framebuffer* fb, u32 argb);
