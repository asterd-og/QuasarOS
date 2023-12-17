#include <exec/elf/elf.h>

/*

Special thanks for: xyve7 (https://github.com/xyve7)

*/

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
            u64 seg_size = div_round_up(phdr->mem_size, page_size);
            char* seg = to_higher_half(pmm_alloc(seg_size));
            memcpy((void*)seg, (void*)addr + phdr->offset, phdr->file_size);
            if (phdr->file_size < phdr->mem_size) {
                memset((void*)seg + phdr->file_size, 0, phdr->mem_size - phdr->file_size);
            }
            vmm_map(page_map, (uptr)to_physical(seg), phdr->vaddr, vmm_flag_present | vmm_flag_exec | vmm_flag_write);
        }
    }

    return hdr->entry;
}