#include <exec/elf/elf.h>

u64 elf_exec(char* addr, page_map* page_map) {
    elf_header* hdr = (elf_header*)addr;

    if (hdr->magic[0] != 0x7f || hdr->magic[1] != 'E' ||
        hdr->magic[2] != 'L' || hdr->magic[3] != 'F') {
            serial_printf("Invalid ELF Header!\n");
            return 0;
        }
    
    if (hdr->type != 2) {
        serial_printf("ELF Not executable!\n");
        return 0;
    }

    elf_program_header* phdr = (elf_program_header*)((char*)addr + hdr->phoff);
    for (u64 i = 0; i < hdr->entry_PHCount; i++, phdr++) {
        if (phdr->type == ELF_LOAD) {
            vmm_alloc_map_pages(page_map, align_up(phdr->mem_size / page_size, page_size), phdr->vaddr, vmm_flag_present | vmm_flag_exec | vmm_flag_write);
            memcpy((void*)phdr->vaddr, (void*)addr + phdr->offset, phdr->file_size);
        }
    }

    return hdr->entry;
}