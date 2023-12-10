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

    //u64 size = 0;
    ELF_ProgramHeader* pHdr = (ELF_ProgramHeader*)((char*)addr + hdr->phoff);
    for (u64 i = 0; i < hdr->entryPhCount; i++, pHdr++) {
        if (pHdr->type == ELF_PHDRLoad) {
            Serial_Printf("Found PHDR!\n");
            //
        }
    }

    //void* ret = Heap_Alloc(4096);
    //memcpy(ret, hdr->entry, );

    return 0;
}