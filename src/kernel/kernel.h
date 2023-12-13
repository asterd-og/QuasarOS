#pragma once

#include <limine.h>
#include <types.h>

extern u64 Kernel_End;
extern u64 HHDM_Offset;

extern volatile struct limine_framebuffer_request fb_request;

extern volatile struct limine_hhdm_request hhdm_request;
