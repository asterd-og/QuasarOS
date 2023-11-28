#include <arch/x86_64/mm/pmm.h>

u8* PMM_Bitmap;
u64 PMM_BitmapSize;
u64 PMM_BitmapIndex;
u64 PMM_Pages;
u64 PMM_FreeMemory;

static volatile struct limine_memmap_request mmapReq = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

void PMM_Init() {
    u64 higherAddr;
    u64 topAddr;

    struct limine_memmap_response* MMap_Data = mmapReq.response;

    for (u64 i = 0; i < MMap_Data->entry_count; i++) {
        Serial_Printf("%x - %lx | %ld\n", MMap_Data->entries[i]->type,
            MMap_Data->entries[i]->base, MMap_Data->entries[i]->length);
        if (MMap_Data->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            topAddr = MMap_Data->entries[i]->base +
                      MMap_Data->entries[i]->length;
            
            if (topAddr > higherAddr) {
                higherAddr = topAddr;
            }
        }
    }

    PMM_FreeMemory = higherAddr;
    PMM_Pages = higherAddr / pageSize;
    PMM_BitmapSize = alignUp(PMM_Pages / 8, pageSize);

    for (u64 i = 0; i < MMap_Data->entry_count; i++) {
        if (MMap_Data->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            if (MMap_Data->entries[i]->length >= PMM_BitmapSize) {
                PMM_Bitmap = (u8*)toHigherHalf(MMap_Data->entries[i]->base);
                memset(PMM_Bitmap, 0xFF, PMM_BitmapSize);
                MMap_Data->entries[i]->base += PMM_BitmapSize;
                MMap_Data->entries[i]->length -= PMM_BitmapSize;
                break;
            }
        }
    }

    for (u64 i = 0; i < MMap_Data->entry_count; ++i) {
        if (MMap_Data->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            for (u64 j = 0; j < MMap_Data->entries[i]->length; j += pageSize) {
                Bitmap_ClearBit(PMM_Bitmap, (MMap_Data->entries[i]->base + j) / pageSize);
            }
        }
    }
}

void* PMM_Alloc(u64 pages) {
    u64 pagesFree = 0;

    for (;PMM_BitmapIndex < PMM_Pages; PMM_BitmapIndex++) {
        if (PMM_BitmapIndex > PMM_Pages) {
            Serial_Printf("PMM_Alloc: Out of memory.\n");
            return (void*)-1;
        } else if (pagesFree == pages) {
            break;
        } else if (Bitmap_GetBit(PMM_Bitmap, PMM_BitmapIndex) == 0) {
            pagesFree++;
        } else {
            pagesFree = 0;
        }
    }
    
    for (u64 i = 0; i < pages; i++) {
       Bitmap_SetBit(PMM_Bitmap, (PMM_BitmapIndex - pagesFree) + i);
    }

    return (void*)((PMM_BitmapIndex - pagesFree) * pageSize);
}

void PMM_Free(void* address, u64 pages) {
    u64 pageIndex = (u64)address;
    pageIndex /= pageSize;

    for (u64 i = 0; i < pages; i++) {
        Bitmap_ClearBit(PMM_Bitmap, pageIndex + i);
    }

    PMM_BitmapIndex = pageIndex;
    return;
}

u64 PMM_GetFreeMemory() {
    return PMM_FreeMemory;
}