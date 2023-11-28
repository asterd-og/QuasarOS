#pragma once

#include <limine.h>
#include <types.h>

extern u64 Kernel_End;
extern u64 HHDM_Offset;

extern struct flanterm_context *pFtCtx;

extern volatile struct limine_framebuffer_request fbReq;
extern struct limine_framebuffer_response* pFbRes;

extern volatile struct limine_hhdm_request hhdmReq;
