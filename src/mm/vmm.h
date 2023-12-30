#pragma once

#include <types.h>
#include <mm/pmm.h>
#include <kernel/kernel.h>
#include <limine.h>

#define vmm_flag_present 0b1
#define vmm_flag_write (0b1ull << 1)
#define vmm_flag_user (0b1ull << 2)
#define vmm_flag_no_exec (0b1ull << 63)
#define vmm_flag_exec 0x04

typedef uptr page_map;

extern page_map* vmm_current_page_map;
extern page_map* page_map_kernel;

extern symbol text_start;
extern symbol text_end;

extern symbol rodata_start;
extern symbol rodata_end;

extern symbol data_start;
extern symbol data_end;

u64 read_cr3();

void vmm_init();
void* vmm_alloc_map_pages(page_map* page_map, u64 pages, uptr vaddr, uptr flags);
void* vmm_alloc(page_map* page_map, u64 pages, uptr flags);
void* vmm_free(page_map* page_map, void* ptr, u64 pages);
void vmm_map(page_map* page_map, uptr paddr, uptr vaddr, uptr flags);
void vmm_unmap(page_map* page_map, uptr vaddr);

page_map* page_map_new();
void page_map_delete(page_map* page_map);
void page_map_load(page_map* page_map);

void mmap(void* vaddr, size_t len, uptr flags, bool shared);