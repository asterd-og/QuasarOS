#pragma once

#include <video/fb.h>
#include <limine.h>

void VBE_Init();
void VBE_Update();
u32* VBE_GetAddr();

extern Framebuffer* VBE;