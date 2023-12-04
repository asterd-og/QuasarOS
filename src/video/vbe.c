#include <video/vbe.h>
#include <libc/lock.h>

static Locker VBE_Lock;

volatile struct limine_framebuffer_request FB_Req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

Framebuffer* VBE;
u32* FB_Addr;

void VBE_Init() {
    struct limine_framebuffer* FB_Res = FB_Req.response->framebuffers[0];
    FB_Addr = FB_Res->address;

    VBE = FB_CreateNewFB(0, 0, FB_Res->width, FB_Res->height, FB_Res->pitch);
}

void VBE_Update() {
    Lock(&VBE_Lock);
    for (u32 i = 0; i < VBE->width * VBE->height; i++)
        FB_Addr[i] = VBE->buffer[i];
    Unlock(&VBE_Lock);
}

u32* VBE_GetAddr() {
    return FB_Addr;
}