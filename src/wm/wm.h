#pragma once

#include <types.h>
#include <video/fb.h>
#include <video/vbe.h>

#define WM_MAX_WINDOWS 16

typedef struct {
    char* title;
    framebuffer* fb;
    u32 id;
} wm_window;

void wm_init();
wm_window* wm_create_new_window(char* title, u32 width, u32 height);
void wm_tick(); // This should be started up on tasking.