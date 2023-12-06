#include <exec/elf/elf.h>
#include <libc/printf.h>
#include <heap/heap.h>
#include <arch/x86_64/mm/pmm.h>
#include <arch/x86_64/mm/vmm.h>

// #define ELF_Base 0x100000000
#define ELF_Base 0x400000

u64 ELF_Exec(u64* addr) {
    ELF_Header* hdr = (ELF_Header*)addr;

    if (hdr->magic[0] != 0x7f || hdr->magic[1] != 'E' ||
        hdr->magic[2] != 'L' || hdr->magic[3] != 'F') {
            Serial_Printf("Invalid ELF Header!\n");
            return 0;
        }
    
    if (hdr->type != 2) {
        Serial_Printf("ELF Not executable!\n");
        return 0;
    }
    
    void* baseAddr;
    u64 vAddrStart;
    u64 vAddrEnd;
    u64 ret;

    ELF_ProgramHeader* pHdr = (ELF_ProgramHeader*)((char*)addr + hdr->phoff);
    for (u64 i = 0; i < hdr->entryPhCount; i++, pHdr++) {
        if (pHdr->type == ELF_PHDRLoad) {
            Serial_Printf("Found PHDR!\n");
            vAddrStart = pHdr->vaddr & ~(pageSize - 1);
            vAddrEnd = (pHdr->vaddr + pHdr->memSize + pageSize - 1) & ~(pageSize - 1);
            baseAddr = Heap_PAlloc(pHdr->memSize);
            for (u64 i = vAddrStart; i < vAddrEnd; i += pageSize) {
                PageMap_Virt2Phys(VMM_CurrentPageMap, (uptr)baseAddr + i, i, VMM_FlagPresent | VMM_FlagWrite);
            }
            memcpy((void*)vAddrStart, addr + pHdr->offset, pHdr->fileSize);
            if (pHdr->flags == ELF_FlagX + ELF_FlagR + ELF_FlagW || pHdr->flags == ELF_FlagX + ELF_FlagR) {
                ret = hdr->entry + vAddrStart;
                Serial_Printf("Found program start at %lx\n", ret);
            }
        }
    }

    return ret;
}