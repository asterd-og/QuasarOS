#pragma once

#include <types.h>
#include <libc/printf.h>
#include <video/vbe.h>
#include <wm/wm.h>

void panic(const char* fmt, ...);