#include <heap/heap.h>

uptr Heap_Start;
uptr Heap_End;
uptr* Heap_Addr;

int Heap_Init(uptr startAddr) {
    if (startAddr <= 0) return 1;
    Heap_Start = startAddr;
    Heap_End = Heap_Start + PMM_GetFreeMemory();
    Heap_Addr = (uptr*)startAddr;

    Serial_Printf("Heap initialised at 0x%lx | ends at 0x%lx | 0x%lx\n",
        Heap_Start, Heap_End, (uptr)Heap_Addr);

    return 0;
}

void* Heap_Alloc(u64 size) {
    Heap_MemoryBlockHDR* block = (Heap_MemoryBlockHDR*)Heap_Addr;
    if (block->magic == Heap_Magic) {
        bool found = false;
        while ((uptr)Heap_Addr < Heap_End) {
            Heap_Addr += block->size + sizeof(Heap_MemoryBlockHDR);
            block = (Heap_MemoryBlockHDR*)Heap_Addr;
            if (block->magic != Heap_Magic) {
                found = true;
                break;
            }
        }
        if (!found) {
            Serial_Printf("Heap: Out of memory.\n");
            return (void*)-1;
        }
    }
    block->magic = Heap_Magic;
    block->size = size;
    Serial_Printf("Heap: Allocated %ld bytes at 0x%lx\n", size, Heap_Addr);
    Heap_Addr += size;
    return (void*)Heap_Addr - size;
}

void Heap_Free(void* ptr) {
    Heap_MemoryBlockHDR* block = (Heap_MemoryBlockHDR*)(ptr - sizeof(Heap_MemoryBlockHDR));
    block->magic = 0;
    block->size = 0;
}

void* Heap_PAlloc(u64 size) {
    return PMM_Alloc((size > pageSize) ? divRoundUp(size / pageSize, pageSize) : 1);
}

void* Heap_VAlloc(VMM_PageMap* pageMap, u64 size) {
    return VMM_AllocPages(pageMap, (size > pageSize) ? divRoundUp(size / pageSize, pageSize) : 1, VMM_FlagPresent | VMM_FlagWrite);
}