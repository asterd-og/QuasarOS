#pragma once

#include <video/fb.h>
#include <limine.h>

void VBE_Init();
void VBE_Update();

extern Framebuffer* VBE;