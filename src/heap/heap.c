#include <heap/heap.h>
#include <libc/panic.h>

/*

KERNEL (SHARED) ALLOCATIONS:
kmalloc
kfree

*/

void* kmalloc(size_t size) {
    u64 pages = align_down(size + sizeof(heap_memory_block), page_size) / page_size;
    heap_memory_block* memory_block = (heap_memory_block*)vmm_alloc(page_map_kernel, page_size, vmm_flag_present | vmm_flag_write);
    memory_block->magic = HEAP_MAGIC;
    memory_block->pages = pages;
    return (memory_block + 1);
}

void kfree(void* ptr) {
    heap_memory_block* memory_block = (heap_memory_block*)(ptr - sizeof(heap_memory_block));
    memset(ptr, 0, memory_block->pages * page_size);
    memory_block->pages = 0; // Sets block as usable.
}

/*

USER ALLOCATIONS:
malloc
free

*/

void* malloc(size_t size) {
    u64 pages = align_down(size + sizeof(heap_memory_block), page_size) / page_size;
    heap_memory_block* memory_block = (heap_memory_block*)vmm_alloc(vmm_current_page_map, page_size, vmm_flag_present | vmm_flag_write);
    memory_block->magic = HEAP_MAGIC;
    memory_block->pages = pages;
    return (memory_block + 1);
}

void free(void* ptr) {
    heap_memory_block* memory_block = (heap_memory_block*)(ptr - sizeof(heap_memory_block));
    memset(ptr, 0, memory_block->pages * page_size);
    memory_block->pages = 0; // Sets block as usable.
}