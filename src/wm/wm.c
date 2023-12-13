#include <wm/wm.h>
#include <heap/heap.h>
#include <sched/sched.h>

wm_window* wm_window_list[WM_MAX_WINDOWS] = {0};
int wm_id = 0;

void wm_init() {
}

void wm_draw_window(wm_window* window) {
    fb_fill_rect(vbe, window->fb->x, window->fb->y - 16, window->fb->width, 16, 0xff2a2a2a);
    fb_draw_string(vbe, font, window->title, window->fb->x, window->fb->y - font->header->height, 0xffd1d1d1);
    fb_copy(window->fb, vbe);
}

void wm_tick() {
    for (int i = 0; i < wm_id; i++) {
        wm_draw_window(wm_window_list[i]);
    }
}

wm_window* wm_create_new_window(char* title, u32 width, u32 height) {
    if (wm_id > WM_MAX_WINDOWS) {
        return NULL;
    }

    wm_window* window = (wm_window*)kmalloc(sizeof(wm_window));
    window->title = title;
    window->fb = fb_create_new(10 * wm_id, 100, width, height, vbe->pitch);
    window->id = wm_id;

    wm_window_list[wm_id] = window;

    wm_id++;

    return window;
}