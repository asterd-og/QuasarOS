#include <arch/x86_64/mm/vmm.h>

static volatile struct limine_kernel_address_request Kernel_AddrReq = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

VMM_PageMap* PageMap_Kernel;
VMM_PageMap* VMM_CurrentPageMap;

uptr* PageMap_WalkEntry(uptr* directory, uptr entry, uptr flags) {
    if (directory[entry] & VMM_FlagPresent) {
        // If the page exists
        return toHigherHalf(directory[entry] & 0x000FFFFFFFFFF000);
        // We mask and return the address of the page table entry!
    } else {
        // If it wasn't found
        void* pml = (uptr*)PMM_Alloc(1);
        memset(toHigherHalf(pml), 0, pageSize);
        directory[entry] = (uptr)pml | flags;
        return toHigherHalf(pml);
    }
}

void* PageMap_AllocPage(VMM_PageMap* pageMap) {
    (void)pageMap;
    return 0;
}

void PageMap_Virt2Phys(VMM_PageMap* pageMap, uptr physAddr, uptr virtAddr, uptr flags) {
    uptr pml1Entry = (virtAddr >> 12) & 0x1ff;
    uptr pml2Entry = (virtAddr >> 21) & 0x1ff;
    uptr pml3Entry = (virtAddr >> 30) & 0x1ff;
    uptr pml4Entry = (virtAddr >> 39) & 0x1ff; // pageMap

    uptr* pml3 = PageMap_WalkEntry(pageMap, pml4Entry, VMM_FlagPresent | VMM_FlagWrite); // pml4[pml4Entry] = pml3
    uptr* pml2 = PageMap_WalkEntry(pml3, pml3Entry, VMM_FlagPresent | VMM_FlagWrite);     // pml3[pml3Entry] = pml2
    uptr* pml1 = PageMap_WalkEntry(pml2, pml2Entry, VMM_FlagPresent | VMM_FlagWrite);     // pml2[pml2Entry] = pml1

    pml1[pml1Entry] = physAddr | flags;
}

void VMM_Init() {
    PageMap_Kernel = (VMM_PageMap*)PMM_Alloc(1);
    memset(toHigherHalf(PageMap_Kernel), 0, pageSize);

    uptr physBase = Kernel_AddrReq.response->physical_base;
    uptr virtBase = Kernel_AddrReq.response->virtual_base;

    uptr Text_Start = alignDown((uptr)Text_StartAddr, pageSize);
    uptr Text_End   = alignUp((uptr)Text_EndAddr, pageSize);

    uptr Rodata_Start = alignDown((uptr)Rodata_StartAddr, pageSize);
    uptr Rodata_End   = alignUp((uptr)Rodata_EndAddr, pageSize);

    uptr Data_Start = alignDown((uptr)Data_StartAddr, pageSize);
    uptr Data_End   = alignUp((uptr)Data_EndAddr, pageSize);

    for (uptr text = Text_Start; text < Text_End; text += pageSize) {
        PageMap_Virt2Phys(PageMap_Kernel, text - virtBase + physBase, text, VMM_FlagPresent);
    }

    for (uptr rodata = Rodata_Start; rodata < Rodata_End; rodata += pageSize) {
        PageMap_Virt2Phys(PageMap_Kernel, rodata - virtBase + physBase, rodata, VMM_FlagPresent | VMM_FlagNoExec);
    }

    for (uptr data = Data_Start; data < Data_End; data += pageSize) {
        PageMap_Virt2Phys(PageMap_Kernel, data - virtBase + physBase, data, VMM_FlagPresent | VMM_FlagWrite | VMM_FlagNoExec);
    }

    for (uptr gb4 = 0x1000; gb4 < 0x100000000; gb4 += pageSize) {
        PageMap_Virt2Phys(PageMap_Kernel, gb4, gb4, VMM_FlagPresent | VMM_FlagWrite);
        PageMap_Virt2Phys(PageMap_Kernel, gb4, (uptr)toHigherHalf(gb4), VMM_FlagPresent | VMM_FlagWrite);
    }

    VMM_CurrentPageMap = PageMap_Kernel;

    asm volatile("mov %0, %%cr3": :"r"((uptr)PageMap_Kernel) : "memory");
}
