#pragma once

#include <types.h>
#include <arch/x86_64/mm/pmm.h>
#include <kernel/kernel.h>
#include <limine.h>

#define VMM_FlagPresent 0b1
#define VMM_FlagWrite (0b1ull << 1)
#define VMM_FlagUser (0b1ull << 2)
#define VMM_FlagNoExec (0b1ull << 63)

typedef uptr VMM_PageMap;

extern VMM_PageMap* VMM_CurrentPageMap;

extern symbol Text_StartAddr;
extern symbol Text_EndAddr;

extern symbol Rodata_StartAddr;
extern symbol Rodata_EndAddr;

extern symbol Data_StartAddr;
extern symbol Data_EndAddr;

void VMM_Init();
void PageMap_Virt2Phys(VMM_PageMap* pageMap, uptr physAddr, uptr virtAddr, uptr flags);
void* PageMap_AllocPage(VMM_PageMap* pageMap);