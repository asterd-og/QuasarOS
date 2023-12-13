#pragma once

#include <types.h>
#include <kernel/kernel.h>
#include <limine.h>
#include <mm/bitmap.h>
#include <arch/x86_64/cpu/serial.h>
#include <libc/string.h>

#define to_higher_half(ptr) ((void*)(ptr) + hhdm_request.response->offset)
#define to_physical(ptr) ((void*)(ptr) - hhdm_request.response->offset)

extern struct limine_memmap_response mmap_data;

void  pmm_init();
void* pmm_alloc(u64 pages);
void  pmm_free(void* address, u64 pages);
u64   pmm_get_free_memory();