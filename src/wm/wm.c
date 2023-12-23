#include <wm/wm.h>
#include <heap/heap.h>
#include <sched/sched.h>
#include <drivers/mouse.h>

wm_window* wm_window_list[WM_MAX_WINDOWS] = {0};
int wm_id = 0;

u8 wm_cursor[] = {
    1,0,0,0,0,0,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,0,0,0,0,
    1,2,1,0,0,0,0,0,0,0,0,0,
    1,2,2,1,0,0,0,0,0,0,0,0,
    1,2,2,2,1,0,0,0,0,0,0,0,
    1,2,2,2,2,1,0,0,0,0,0,0,
    1,2,2,2,2,2,1,0,0,0,0,0,
    1,2,2,2,2,2,2,1,0,0,0,0,
    1,2,2,2,2,2,2,2,1,0,0,0,
    1,2,2,2,2,2,2,2,2,1,0,0,
    1,2,2,2,2,2,2,2,2,2,1,0,
    1,2,2,2,2,2,2,2,2,2,2,1,
    1,2,2,2,2,2,2,1,1,1,1,1,
    1,2,2,2,1,2,2,1,0,0,0,0,
    1,2,2,1,0,1,2,2,1,0,0,0,
    1,2,1,0,0,1,2,2,1,0,0,0,
    1,1,0,0,0,0,1,2,2,1,0,0,
    0,0,0,0,0,0,1,2,2,1,0,0,
    0,0,0,0,0,0,0,1,1,0,0,0
};

void wm_init() {
}

void wm_draw_cursor() {
    for (u8 y = 0; y < 19; y++) {
        for (u8 x = 0; x < 12; x++) {
            switch (wm_cursor[y * 12 + x]) {
                case 1:
                    fb_set_pix(vbe, x + mouse_x, y + mouse_y, 0xff000000);
                    break;
                case 2:
                    fb_set_pix(vbe, x + mouse_x, y + mouse_y, 0xffffffff);
                    break;
            }
        }
    }
}

void wm_draw_window(wm_window* window) {
    fb_fill_rect(vbe, window->fb->x, window->fb->y - font->header->height, window->fb->width, font->header->height, 0xff2a2a2a);
    fb_draw_string(vbe, font, window->title, window->fb->x, window->fb->y - font->header->height, 0xffd1d1d1);
    fb_copy(window->fb, vbe);
}

void wm_update_window(wm_window* window) {
    if (mouse_left_pressed) {
        if (!window->moving) {
            if (mouse_x >= window->fb->x && mouse_x <= window->fb->x + window->fb->width && mouse_y >= window->fb->y - 16 && mouse_y <= window->fb->y) {
                window->x_offset = mouse_x - window->fb->x;
                window->y_offset = mouse_y - window->fb->y;
                window->moving = true;
            }
        }
    } else {
        window->moving = false;
    }

    if (window->moving) {
        window->fb->x = mouse_x - window->x_offset;
        window->fb->y = mouse_y - window->y_offset;
    }
}

void wm_tick() {
    for (int i = 0; i < wm_id; i++) {
        wm_update_window(wm_window_list[i]);
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
    window->moving = false;

    wm_window_list[wm_id] = window;

    wm_id++;

    return window;
}

void wm_panic_destroy() {
    for (int i = 0; i < wm_id; i++) {
        kfree(wm_window_list[wm_id]);
    }
    wm_id = 0;
}