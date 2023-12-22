#include <video/fb.h>
#include <libc/string.h>
#include <libc/printf.h>
#include <sched/sched.h>

framebuffer* fb_create_new(u32 x, u32 y, u32 width, u32 height, u32 pitch) {
    framebuffer* fb = (framebuffer*)kmalloc(sizeof(framebuffer));
    fb->buffer = (u32*)kmalloc(width * height * 4);
    fb->x = x;
    fb->y = y;
    fb->width = width;
    fb->height = height;
    fb->pitch = pitch;
    memset(fb->buffer, 0, width * height * 4);
    return fb;
}

void fb_set_pix(framebuffer* fb, u32 x, u32 y, u32 argb) {
    if (x > fb->width || x <= 0 || y > fb->height || y <= 0) return;
    fb->buffer[y * fb->pitch / 4 + x] = argb;
}

u32 fb_get_pix(framebuffer* fb, u32 x, u32 y) {
    return fb->buffer[y * fb->pitch / 4 + x];
}

void fb_copy(framebuffer* from, framebuffer* to) {
    sched_lock();
    for (u32 y = 0; y < from->height; y++) {
        for (u32 x = 0; x < from->width; x++) {
            fb_set_pix(to, x + from->x, y + from->y, fb_get_pix(from, x, y));
        }
    }
    sched_unlock();
}

void fb_fill_rect(framebuffer* fb, u32 x, u32 y, u32 width, u32 height, u32 argb) {
    sched_lock();

    for (u32 yy = 0; yy < height; yy++) {
        for (u32 xx = 0; xx < width; xx++) {
            fb_set_pix(fb, x + xx, y + yy, argb);
        }
    }

    sched_unlock();
}

void fb_draw_char(framebuffer* fb, psf2_font* font, char ch, u32 x, u32 y, u32 argb) {
    sched_lock();

    u8* c = font->addr_start + ch * font->header->char_size;

    for (u32 yy = 0; yy < font->header->height; yy++) {
        for (u32 xx = 0; xx < font->header->width; xx++) {
            if ((c[yy * font->pitch + xx / 8] >> (7 - xx % 8)) & 1) {
                fb_set_pix(fb, x + xx, y + yy, argb);
            }
        }
    }

    sched_unlock();
}

void fb_draw_string(framebuffer* fb, psf2_font* font, char* str, u32 x, u32 y, u32 argb) {
    sched_lock();

    for (int i = 0; i < strlen(str); i++) {
        fb_draw_char(fb, font, str[i], x + (i * font->header->width), y, argb);
    }

    sched_unlock();
}

void fb_clear(framebuffer* fb, u32 argb) {
    sched_lock();
    for (u32 i = 0; i < fb->width * fb->height; i++) fb->buffer[i] = argb;
    sched_unlock();
}