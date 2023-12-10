#pragma once

#include <types.h>
#include <arch/x86_64/mm/pmm.h>
#include <kernel/kernel.h>
#include <limine.h>

#define VMM_FlagPresent 0b1
#define VMM_FlagWrite (0b1ull << 1)
#define VMM_FlagUser (0b1ull << 2)
#define VMM_FlagNoExec (0b1ull << 63)
#define VMM_FlagExec 0x04

#define VMM_MapAnon 0x08

typedef uptr VMM_PageMap;

extern VMM_PageMap* VMM_CurrentPageMap;

extern symbol Text_StartAddr;
extern symbol Text_EndAddr;

extern symbol Rodata_StartAddr;
extern symbol Rodata_EndAddr;

extern symbol Data_StartAddr;
extern symbol Data_EndAddr;

void VMM_Init();
void* VMM_AllocPages(VMM_PageMap* pageMap, u64 pages, uptr vaddr, uptr flags);
void* VMM_FreePages(VMM_PageMap* pageMap, void* ptr, u64 pages);

VMM_PageMap* PageMap_New();
void PageMap_Map(VMM_PageMap* pageMap, uptr physAddr, uptr virtAddr, uptr flags);
void PageMap_Unmap(VMM_PageMap* pageMap, uptr virtAddr);
void PageMap_Load(VMM_PageMap* pageMap);