#include <mm/vmm.h>
#include <sched/sched.h>

static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

static volatile struct limine_kernel_file_request kernel_file_request = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0,
};

page_map* page_map_kernel;
page_map* vmm_current_page_map;

uptr phys_base;
uptr virt_base;
u64 kernel_size;

void write_cr3(u64 val) {
    __asm__ volatile("mov %0, %%cr3" :: "r"(val) : "memory");
}

u64 read_cr3() {
    u64 val;
    __asm__ volatile("mov %%cr3, %0" :"=r"(val) :: "memory");
    return val;
}

uptr* page_map_walk_entry(uptr* directory, uptr entry, uptr flags, bool alloc_if_null) {
    if (directory == NULL) {
        serial_printf("page_map_walk_entry: Directory is NULL.\n");
        return NULL;
    }
    if (directory[entry] & vmm_flag_present) {
        // If the page exists
        return to_higher_half(directory[entry] & 0x000FFFFFFFFFF000);
        // We mask and return the address of the page table entry!
    } else {
        // If it wasn't found
        if (alloc_if_null) {
            void* pml = (uptr*)pmm_alloc(1);
            if (pml == NULL) {
                serial_printf("page_map_walk_entry: Couldn't alloc PML.\n");
            }
            memset(to_higher_half(pml), 0, page_size);
            directory[entry] = (uptr)pml | flags;
            return to_higher_half(pml);
        }
        return NULL;
    }
}

void* vmm_alloc_map_pages(page_map* page_map, u64 pages, uptr vaddr, uptr flags) {
    uptr addr = (uptr)pmm_alloc(pages);
    addr = align_down(addr, page_size);
    
    uptr virt;
    uptr phys;

    for (u64 i = 0; i < pages; i++) {
        virt = vaddr + (i * page_size);
        phys = addr + (i * page_size);
        vmm_map(page_map, phys, virt, flags);
    }

    return (void*)addr;
}

void* vmm_alloc(page_map* page_map, u64 pages, uptr flags) {
    uptr addr = (uptr)pmm_alloc(pages);
    addr = align_down(addr, page_size);
    serial_printf("allocated vmm at %lx\n", addr);
    
    uptr virt;
    uptr phys;

    for (u64 i = 0; i < pages; i++) {
        virt = addr + (i * page_size);
        phys = addr + (i * page_size);
        vmm_map(page_map, phys, (uptr)to_higher_half(virt), flags);
    }

    return (void*)to_higher_half(addr);
}

void* vmm_free(page_map* page_map, void* ptr, u64 pages) {
    uptr virt;
    for (u64 i = 0; i < pages; i++) {
        virt = (uptr)ptr + (i * page_size);
        vmm_unmap(page_map, virt);
    }
    pmm_free(ptr, pages);
    return 0;
}

page_map* page_map_new() {
    page_map* pm = (page_map*)to_higher_half(pmm_alloc(1));
    memset(pm, 0, page_size);

    // Create a new page map and copy contents of kernel page map to new one
    for (usize i = 256; i < 512; i++) {
        pm[i] = page_map_kernel[i];
    }
    return pm;
}

void page_map_delete(page_map* page_map) {
    if (vmm_current_page_map == page_map) {
        page_map_load(page_map_kernel);
    }

    memset(page_map, 0, page_size);
    pmm_free((void*)to_physical(page_map), 1);
    return;
}

void vmm_map(page_map* page_map, uptr paddr, uptr vaddr, uptr flags) {
    if (page_map == NULL) {
        serial_printf("vmm_map: page_map is NULL.\n");
    }

    uptr pml1_entry = (vaddr >> 12) & 0x1ff;
    uptr pml2_entry = (vaddr >> 21) & 0x1ff;
    uptr pml3_entry = (vaddr >> 30) & 0x1ff;
    uptr pml4_entry = (vaddr >> 39) & 0x1ff; // page_map

    uptr* pml3 = page_map_walk_entry(page_map, pml4_entry, vmm_flag_present | vmm_flag_write, true); // pml4[pml4Entry] = pml3
    uptr* pml2 = page_map_walk_entry(pml3, pml3_entry, vmm_flag_present | vmm_flag_write, true);    // pml3[pml3Entry] = pml2
    uptr* pml1 = page_map_walk_entry(pml2, pml2_entry, vmm_flag_present | vmm_flag_write, true);    // pml2[pml2Entry] = pml1

    pml1[pml1_entry] = paddr | flags;
}

void vmm_unmap(page_map* page_map, uptr vaddr) {
    uptr pml1_entry = (vaddr >> 12) & 0x1ff;
    uptr pml2_entry = (vaddr >> 21) & 0x1ff;
    uptr pml3_entry = (vaddr >> 30) & 0x1ff;
    uptr pml4_entry = (vaddr >> 39) & 0x1ff; // page_map

    uptr* pml3 = page_map_walk_entry(page_map, pml4_entry, vmm_flag_present | vmm_flag_write, false); // pml4[pml4Entry] = pml3
    if (pml3 == NULL) {
        return;
    }
    uptr* pml2 = page_map_walk_entry(pml3, pml3_entry, vmm_flag_present | vmm_flag_write, false);    // pml3[pml3Entry] = pml2
    if (pml2 == NULL) {
        return;
    }
    uptr* pml1 = page_map_walk_entry(pml2, pml2_entry, vmm_flag_present | vmm_flag_write, false);    // pml2[pml2Entry] = pml1
    if (pml1 == NULL) {
        return;
    }

    // Flush page
    __asm__ volatile("invlpg (%0)" : : "b"(vaddr) : "memory");
    pml1[pml1_entry] = 0;
}

void page_map_load(page_map* page_map) {
    write_cr3((u64)to_physical(page_map));
    vmm_current_page_map = page_map;
}

void vmm_init() {
    page_map_kernel = (page_map*)to_higher_half(pmm_alloc(1));
    memset(page_map_kernel, 0, page_size);
    serial_printf("kernel's page map is: 0x%lx\n", to_physical(page_map_kernel));

    kernel_size = kernel_file_request.response->kernel_file->size;
    phys_base = kernel_address_request.response->physical_base;
    virt_base = kernel_address_request.response->virtual_base;

    uptr text_st   = align_down((uptr)text_start, page_size);
    uptr text_en   = align_up((uptr)text_end, page_size);

    uptr rodata_st = align_down((uptr)rodata_start, page_size);
    uptr rodata_en = align_up((uptr)rodata_end, page_size);

    uptr data_st   = align_down((uptr)data_start, page_size);
    uptr data_en   = align_up((uptr)data_end, page_size);

    for (uptr text = text_st; text < text_en; text += page_size) {
        vmm_map(page_map_kernel, text - virt_base + phys_base, text, vmm_flag_present);
    }

    for (uptr rodata = rodata_st; rodata < rodata_en; rodata += page_size) {
        vmm_map(page_map_kernel, rodata - virt_base + phys_base, rodata, vmm_flag_present | vmm_flag_no_exec);
    }

    for (uptr data = data_st; data < data_en; data += page_size) {
        vmm_map(page_map_kernel, data - virt_base + phys_base, data, vmm_flag_present | vmm_flag_write | vmm_flag_no_exec);
    }

    for (uptr gb4 = 0; gb4 < 0x100000000; gb4 += page_size) {
        vmm_map(page_map_kernel, gb4, gb4, vmm_flag_present | vmm_flag_write);
        vmm_map(page_map_kernel, gb4, (uptr)to_higher_half(gb4), vmm_flag_present | vmm_flag_write);
    }

    page_map_load(page_map_kernel);
}
