#include <exec/flat/flat.h>
#include <heap/heap.h>

void* Flat_Exec(void* addr, size_t size) {
    (void)size;
    return addr;
}