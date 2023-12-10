#pragma once

#include <types.h>
#include <arch/x86_64/mm/pmm.h>
#include <arch/x86_64/mm/vmm.h>

#define Heap_Magic 0xdeadfacade

struct Heap_MemoryBlockHDR {
    u64 magic;
    u64 size;
};

typedef struct Heap_MemoryBlockHDR Heap_MemoryBlockHDR;

int   Heap_Init(uptr startAddr);
void* Heap_Alloc(u64 size);
void  Heap_Free(void* ptr);
void* Heap_PAlloc(u64 size);