#include <arch/x86_64/mm/vmm.h>

#define present 0b1
#define write (0b1ull << 1)
#define user (0b1ull << 2)
#define noExec (0b1ull << 63)

static volatile struct limine_kernel_address_request Kernel_AddrReq = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

uptr* VMM_PML4;

extern u8 Text_StartAddr[];
extern u8 Text_EndAddr[];

extern u8 Rodata_StartAddr[];
extern u8 Rodata_EndAddr[];

extern u8 Data_StartAddr[];
extern u8 Data_EndAddr[];

uptr* VMM_WalkEntry(uptr* directory, uptr entry, uptr flags) {
    if (directory[entry] & present) {
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

void VMM_MapPage(uptr physAddr, uptr virtAddr, uptr flags) {
    uptr pml1Entry = (virtAddr >> 12) & 0x1ff;
    uptr pml2Entry = (virtAddr >> 21) & 0x1ff;
    uptr pml3Entry = (virtAddr >> 30) & 0x1ff;
    uptr pml4Entry = (virtAddr >> 39) & 0x1ff;

    uptr* pml3 = VMM_WalkEntry(VMM_PML4, pml4Entry, present | write); // pml4[pml4Entry] = pml3
    uptr* pml2 = VMM_WalkEntry(pml3, pml3Entry, present | write);     // pml3[pml3Entry] = pml2
    uptr* pml1 = VMM_WalkEntry(pml2, pml2Entry, present | write);     // pml2[pml2Entry] = pml1

    pml1[pml1Entry] = physAddr | flags;
}

void VMM_Init() {
    VMM_PML4 = (uptr*)PMM_Alloc(1);
    memset(toHigherHalf(VMM_PML4), 0, pageSize);

    uptr physBase = Kernel_AddrReq.response->physical_base;
    uptr virtBase = Kernel_AddrReq.response->virtual_base;

    uptr Text_Start = alignDown((uptr)Text_StartAddr, pageSize);
    uptr Text_End   = alignUp((uptr)Text_EndAddr, pageSize);

    uptr Rodata_Start = alignDown((uptr)Rodata_StartAddr, pageSize);
    uptr Rodata_End   = alignUp((uptr)Rodata_EndAddr, pageSize);

    uptr Data_Start = alignDown((uptr)Data_StartAddr, pageSize);
    uptr Data_End   = alignUp((uptr)Data_EndAddr, pageSize);

    for (uptr text = Text_Start; text < Text_End; text += pageSize) {
        VMM_MapPage(text - virtBase + physBase, text, present);
    }

    for (uptr rodata = Rodata_Start; rodata < Rodata_End; rodata += pageSize) {
        VMM_MapPage(rodata - virtBase + physBase, rodata, present | noExec);
    }

    for (uptr data = Data_Start; data < Data_End; data += pageSize) {
        VMM_MapPage(data - virtBase + physBase, data, present | write | noExec);
    }

    for (uptr gb4 = 0x1000; gb4 < 0x100000000; gb4 += pageSize) {
        VMM_MapPage(gb4, gb4, present | write);
        VMM_MapPage(gb4, (uptr)toHigherHalf(gb4), present | write);
    }

    asm volatile("mov %0, %%cr3": :"r"((uptr)VMM_PML4) : "memory");
}
