#pragma once

#include <types.h>
#include <libc/string.h>

#define MAGIC 0xbadcafee

typedef struct {
    char magic[4];
    size_t fileCount;
} quasfs_head;

typedef struct {
    uint64_t magic; // should be 0xbadcafee
    char name[50];
    uint64_t address;
    size_t size;
} quasfs_header;

int quasfs_init(void* addr);
char* quasfs_read(const char* name);
char** quasfs_dir();
size_t quasfs_ftell(const char* name);
int quasfs_get_entry_count();