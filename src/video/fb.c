#include <video/fb.h>
#include <libc/string.h>
#include <libc/lock.h>

static Locker FB_Lock;

Framebuffer* FB_CreateNewFB(u32 x, u32 y,
    u32 width, u32 height, u32 pitch) {
    Lock(&FB_Lock);
    Framebuffer* fb = (Framebuffer*)Heap_Alloc(sizeof(Framebuffer));
    fb->buffer = (u32*)Heap_Alloc(width * height * 4);
    fb->x = x;
    fb->y = y;
    fb->width = width;
    fb->height = height;
    fb->pitch = pitch;
    memset(fb->buffer, 0, width * height * 4);
    Unlock(&FB_Lock);
    return fb;
}

void FB_SetPix(Framebuffer* fb, u32 x, u32 y, u32 argb) {
    Lock(&FB_Lock);
    if (x > fb->width || x < 0 || y > fb->height || y < 0) return;
    fb->buffer[y * fb->pitch / 4 + x] = argb;
    Lock(&FB_Lock);
}

u32 FB_GetPix(Framebuffer* fb, u32 x, u32 y) {
    Lock(&FB_Lock);
    u32 argb = fb->buffer[y * fb->pitch / 4 + x];
    Unlock(&FB_Lock);
    return argb;
}

void FB_CopyFB(Framebuffer* from, Framebuffer* to) {
    Lock(&FB_Lock);
    for (u32 y = 0; y < from->height; y++) {
        for (u32 x = 0; x < from->width; x++) {
            FB_SetPix(to, x + from->x, y + from->y, FB_GetPix(from, x, y));
        }
    }
    Unlock(&FB_Lock);
}

void FB_Clear(Framebuffer* fb, u32 argb) {
    Lock(&FB_Lock);
    for (u32 i = 0; i < fb->width * fb->height; i++) fb->buffer[i] = argb;
    Unlock(&FB_Lock);
}