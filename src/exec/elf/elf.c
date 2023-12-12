#include <exec/elf/elf.h>

u64 ELF_Exec(char* addr, VMM_PageMap* pageMap) {
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

    ELF_ProgramHeader* pHdr = (ELF_ProgramHeader*)((char*)addr + hdr->phoff);
    for (u64 i = 0; i < hdr->entryPhCount; i++, pHdr++) {
        if (pHdr->type == ELF_PHDRLoad) {
            VMM_AllocMapPages(pageMap, alignUp(pHdr->memSize / pageSize, pageSize), pHdr->vaddr, VMM_FlagPresent | VMM_FlagExec | VMM_FlagWrite);
            memcpy((void*)pHdr->vaddr, (void*)addr + pHdr->offset, pHdr->fileSize);
        }
    }

    return hdr->entry;
}