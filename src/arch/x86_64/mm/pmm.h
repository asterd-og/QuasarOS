#pragma once

#include <types.h>
#include <kernel/kernel.h>
#include <limine.h>
#include <arch/x86_64/mm/bitmap.h>
#include <arch/x86_64/cpu/serial.h>
#include <libc/string.h>

#define toHigherHalf(ptr) ((void*)(ptr) + hhdmReq.response->offset)

void  PMM_Init();
void* PMM_Alloc(u64 pages);
void  PMM_Free(void* address, u64 pages);
u64   PMM_GetFreeMemory();