#include <mm/pmm.h>
#include <libc/panic.h>

u8* pmm_bitmap;
u64 pmm_bitmap_size;
u64 pmm_bitmap_index;
u64 pmm_pages;
u64 pmm_free_memory;

static volatile struct limine_memmap_request mmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

struct limine_memmap_response mmap_data;

void pmm_init() {
    u64 higher_address = 0;
    u64 top_address = 0;

    struct limine_memmap_response* pmm_mmap_data = mmap_request.response;
    mmap_data = *pmm_mmap_data;

    for (u64 i = 0; i < pmm_mmap_data->entry_count; i++) {
        if (pmm_mmap_data->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            top_address = pmm_mmap_data->entries[i]->base +
                      pmm_mmap_data->entries[i]->length;
            
            if (top_address > higher_address) {
                higher_address = top_address;
            }
        }
    }

    pmm_free_memory = higher_address;
    pmm_pages = higher_address / page_size;
    pmm_bitmap_size = align_up(pmm_pages / 8, page_size);

    for (u64 i = 0; i < pmm_mmap_data->entry_count; i++) {
        if (pmm_mmap_data->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            if (pmm_mmap_data->entries[i]->length >= pmm_bitmap_size) {
                pmm_bitmap = (u8*)to_higher_half(pmm_mmap_data->entries[i]->base);
                memset(pmm_bitmap, 0xFF, pmm_bitmap_size);
                pmm_mmap_data->entries[i]->base += pmm_bitmap_size;
                pmm_mmap_data->entries[i]->length -= pmm_bitmap_size;
                break;
            }
        }
    }

    for (u64 i = 0; i < pmm_mmap_data->entry_count; ++i) {
        if (pmm_mmap_data->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            for (u64 j = 0; j < pmm_mmap_data->entries[i]->length; j += page_size) {
                bitmap_clear(pmm_bitmap, (pmm_mmap_data->entries[i]->base + j) / page_size);
            }
        }
    }
}

void* pmm_alloc(u64 pages) {
    u64 pages_free = 0;

    for (;pmm_bitmap_index < pmm_pages; pmm_bitmap_index++) {
        if (pmm_bitmap_index > pmm_pages) {
            panic("pmm_alloc: Out of memory.\n");
            return (void*)-1;
        } else if (pages_free == pages) {
            break;
        } else if (bitmap_get(pmm_bitmap, pmm_bitmap_index) == 0) {
            pages_free++;
        } else {
            pages_free = 0;
        }
    }
    
    for (u64 i = 0; i < pages; i++) {
       bitmap_set(pmm_bitmap, (pmm_bitmap_index - pages_free) + i);
    }

    return (void*)((pmm_bitmap_index - pages_free) * page_size);
}

void pmm_free(void* address, u64 pages) {
    u64 page_index = (u64)address;
    page_index /= page_size;

    for (u64 i = 0; i < pages; i++) {
        bitmap_clear(pmm_bitmap, page_index + i);
    }

    pmm_bitmap_index = page_index;
    return;
}

u64 pmm_get_free_memory() {
    return pmm_free_memory;
}