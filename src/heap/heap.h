#pragma once

#include <types.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

#define HEAP_MAGIC 0xdeadfacade

typedef struct {
    u64 magic;
    u64 pages;
} heap_memory_block;

void* kmalloc(size_t size);
void  kfree(void* ptr);

void* malloc(size_t size);
void  free(void* ptr);