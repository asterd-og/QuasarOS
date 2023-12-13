#pragma once

#include <types.h>
#include <video/vbe.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/io.h>

extern u32 mouse_x;
extern u32 mouse_y;

extern bool mouse_left_pressed;
extern bool mouse_right_pressed;

void mouse_init();