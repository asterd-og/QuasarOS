#include <heap/heap.h>

void* kmalloc(size_t size) {
    void* address = vmm_alloc(page_map_kernel, align_up(size + sizeof(heap_memory_block), page_size) / page_size, vmm_flag_present | vmm_flag_write);
    heap_memory_block* memory_block = (heap_memory_block*)address;
    memory_block->magic = HEAP_MAGIC;
    memory_block->pages = align_up(size + sizeof(heap_memory_block), page_size) / page_size;
    return address + sizeof(heap_memory_block);
}

void kfree(void* ptr) {
    heap_memory_block* memory_block = (heap_memory_block*)(ptr - sizeof(heap_memory_block));
    vmm_free(page_map_kernel, ptr - sizeof(heap_memory_block), memory_block->pages);
}