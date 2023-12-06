#pragma once

#include <types.h>
#include <video/vbe.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/io.h>

extern u32 Mouse_X;
extern u32 Mouse_Y;

extern bool Mouse_LeftPressed;
extern bool Mouse_RightPressed;

void Mouse_Init();