#include <video/vbe.h>
#include <sched/sched.h>
#include <initrd/quasfs.h>

volatile struct limine_framebuffer_request fb_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

framebuffer* vbe;
u32* vbe_addr;
psf2_font* font;

void vbe_init() {
    struct limine_framebuffer* fb_res = fb_request.response->framebuffers[0];
    vbe_addr = fb_res->address;

    vbe = fb_create_new(0, 0, fb_res->width, fb_res->height, fb_res->pitch);
    font = psf2_load(quasfs_read("kfont.psf"));
}

void vbe_update() {
    sched_lock();
    memcpy(vbe_addr, vbe->buffer, vbe->width * vbe->height * 4);
    /*for (u32 i = 0; i < vbe->width * vbe->height; i++)
        vbe_addr[i] = vbe->buffer[i];*/
    sched_unlock();
}