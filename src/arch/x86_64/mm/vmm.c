#include <arch/x86_64/mm/vmm.h>
#include <sched/sched.h>

static volatile struct limine_kernel_address_request Kernel_AddrReq = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

static volatile struct limine_kernel_file_request Kernel_FileReq = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0,
};

VMM_PageMap* PageMap_Kernel;
VMM_PageMap* VMM_CurrentPageMap;
void* VMM_LastAlloc = 0;

uptr physBase;
uptr virtBase;
u64 kernelSize;

void writeCR3(u64 val) {
    asm volatile("mov %0, %%cr3" :: "r"(val) : "memory");
}

void* readCR3() {
    u64 val;
    asm volatile("mov %%cr3, %0" :"=r"(val) :: "memory");
    return toHigherHalf(val);
}

uptr* PageMap_WalkEntry(uptr* directory, uptr entry, uptr flags, bool allocIfNull) {
    if (directory[entry] & VMM_FlagPresent) {
        // If the page exists
        return toHigherHalf(directory[entry] & 0x000FFFFFFFFFF000);
        // We mask and return the address of the page table entry!
    } else {
        // If it wasn't found
        if (allocIfNull) {
            void* pml = (uptr*)PMM_Alloc(1);
            memset(toHigherHalf(pml), 0, pageSize);
            directory[entry] = (uptr)pml | flags;
            return toHigherHalf(pml);
        }
        return NULL;
    }
}

void* VMM_AllocPages(VMM_PageMap* pageMap, u64 pages, uptr vaddr, uptr flags) {
    uptr addr = (uptr)PMM_Alloc(pages);
    addr = alignDown(addr, pageSize);
    
    uptr virt;
    uptr phys;

    for (u64 i = 0; i < pages; i++) {
        virt = vaddr + (i * pageSize);
        phys = addr + (i * pageSize);
        PageMap_Map(pageMap, phys, virt, flags);
    }

    return (void*)addr;
}

void* VMM_FreePages(VMM_PageMap* pageMap, void* ptr, u64 pages) {
    uptr virt;
    for (u64 i = 0; i < pages; i++) {
        virt = (uptr)ptr + (i * pageSize);
        PageMap_Unmap(pageMap, virt);
    }
    return 0;
}

VMM_PageMap* PageMap_New() {
    VMM_PageMap* pageMap = (VMM_PageMap*)toHigherHalf(PMM_Alloc(1));
    memset(pageMap, 0, pageSize);
    
    // Create a new page map and copy contents of kernel page map to new one
    for (u64 i = 256; i < 512; i++) {
        pageMap[i] = PageMap_Kernel[i];
    }

    u64 start = 0;
    u64 end = 0;

    for (u64 entry = 0; entry < MMap_Data.entry_count; entry++) {
        start = alignDown(MMap_Data.entries[entry]->base, pageSize);
        end = alignUp(MMap_Data.entries[entry]->base + MMap_Data.entries[entry]->length, pageSize);

        for (u64 i = start; i < end; i += pageSize) {
            PageMap_Map(pageMap, i, i, VMM_FlagPresent | VMM_FlagWrite);
            PageMap_Map(pageMap, i, (uptr)toHigherHalf(i), VMM_FlagPresent | VMM_FlagWrite);
        }
    }

    for (u64 i = 0; i < kernelSize; i += pageSize) {
        PageMap_Map(pageMap, physBase + i, virtBase + i, VMM_FlagPresent | VMM_FlagWrite);
    }

    return pageMap;
}

void PageMap_Delete(VMM_PageMap* pageMap) {
    if (VMM_CurrentPageMap == pageMap) {
        return; // For now we just return
    }

    PMM_Free((void*)toPhysical(pageMap), 1);
    memset(pageMap, 0, pageSize);
    return;
}

void PageMap_Map(VMM_PageMap* pageMap, uptr physAddr, uptr virtAddr, uptr flags) {
    uptr pml1Entry = (virtAddr >> 12) & 0x1ff;
    uptr pml2Entry = (virtAddr >> 21) & 0x1ff;
    uptr pml3Entry = (virtAddr >> 30) & 0x1ff;
    uptr pml4Entry = (virtAddr >> 39) & 0x1ff; // pageMap

    uptr* pml3 = PageMap_WalkEntry(pageMap, pml4Entry, VMM_FlagPresent | VMM_FlagWrite, true); // pml4[pml4Entry] = pml3
    uptr* pml2 = PageMap_WalkEntry(pml3, pml3Entry, VMM_FlagPresent | VMM_FlagWrite, true);     // pml3[pml3Entry] = pml2
    uptr* pml1 = PageMap_WalkEntry(pml2, pml2Entry, VMM_FlagPresent | VMM_FlagWrite, true);     // pml2[pml2Entry] = pml1

    pml1[pml1Entry] = physAddr | flags;
}

void PageMap_Unmap(VMM_PageMap* pageMap, uptr virtAddr) {
    uptr pml1Entry = (virtAddr >> 12) & 0x1ff;
    uptr pml2Entry = (virtAddr >> 21) & 0x1ff;
    uptr pml3Entry = (virtAddr >> 30) & 0x1ff;
    uptr pml4Entry = (virtAddr >> 39) & 0x1ff; // pageMap

    uptr* pml3 = PageMap_WalkEntry(pageMap, pml4Entry, VMM_FlagPresent | VMM_FlagWrite, false); // pml4[pml4Entry] = pml3
    uptr* pml2 = PageMap_WalkEntry(pml3, pml3Entry, VMM_FlagPresent | VMM_FlagWrite, false);     // pml3[pml3Entry] = pml2
    uptr* pml1 = PageMap_WalkEntry(pml2, pml2Entry, VMM_FlagPresent | VMM_FlagWrite, false);     // pml2[pml2Entry] = pml1

    pml1[pml1Entry] = 0;
    // Flush page
    asm volatile("invlpg (%0)" : : "b"(virtAddr) : "memory");
}

void PageMap_Load(VMM_PageMap* pageMap) {
    writeCR3((u64)toPhysical(pageMap));
}

void VMM_Init() {
    PageMap_Kernel = (VMM_PageMap*)toHigherHalf(PMM_Alloc(1));
    memset(PageMap_Kernel, 0, pageSize);

    kernelSize = Kernel_FileReq.response->kernel_file->size;
    physBase = Kernel_AddrReq.response->physical_base;
    virtBase = Kernel_AddrReq.response->virtual_base;

    uptr Text_Start = alignDown((uptr)Text_StartAddr, pageSize);
    uptr Text_End   = alignUp((uptr)Text_EndAddr, pageSize);

    uptr Rodata_Start = alignDown((uptr)Rodata_StartAddr, pageSize);
    uptr Rodata_End   = alignUp((uptr)Rodata_EndAddr, pageSize);

    uptr Data_Start = alignDown((uptr)Data_StartAddr, pageSize);
    uptr Data_End   = alignUp((uptr)Data_EndAddr, pageSize);

    for (uptr text = Text_Start; text < Text_End; text += pageSize) {
        PageMap_Map(PageMap_Kernel, text - virtBase + physBase, text, VMM_FlagPresent);
    }

    for (uptr rodata = Rodata_Start; rodata < Rodata_End; rodata += pageSize) {
        PageMap_Map(PageMap_Kernel, rodata - virtBase + physBase, rodata, VMM_FlagPresent | VMM_FlagNoExec);
    }

    for (uptr data = Data_Start; data < Data_End; data += pageSize) {
        PageMap_Map(PageMap_Kernel, data - virtBase + physBase, data, VMM_FlagPresent | VMM_FlagWrite | VMM_FlagNoExec);
    }

    for (uptr gb4 = 0; gb4 < 0x100000000; gb4 += pageSize) {
        PageMap_Map(PageMap_Kernel, gb4, gb4, VMM_FlagPresent | VMM_FlagWrite);
        PageMap_Map(PageMap_Kernel, gb4, (uptr)toHigherHalf(gb4), VMM_FlagPresent | VMM_FlagWrite);
    }

    u64 start = 0;
    u64 end = 0;

    for (u64 entry = 0; entry < MMap_Data.entry_count; entry++) {
        start = alignDown(MMap_Data.entries[entry]->base, pageSize);
        end = alignUp(MMap_Data.entries[entry]->base + MMap_Data.entries[entry]->length, pageSize);

        for (u64 i = start; i < end; i += pageSize) {
            PageMap_Map(PageMap_Kernel, i, i, VMM_FlagPresent | VMM_FlagWrite);
            PageMap_Map(PageMap_Kernel, i, (uptr)toHigherHalf(i), VMM_FlagPresent | VMM_FlagWrite);
        }
    }

    for (u64 i = 0; i < kernelSize; i += pageSize) {
        PageMap_Map(PageMap_Kernel, physBase + i, virtBase + i, VMM_FlagPresent | VMM_FlagWrite);
    }

    PageMap_Load(PageMap_Kernel);
}
