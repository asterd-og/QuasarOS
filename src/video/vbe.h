#pragma once

#include <video/fb.h>
#include <limine.h>

void vbe_init();
void vbe_update();

extern framebuffer* vbe;
extern psf2_font* font;
extern u32* vbe_addr;
