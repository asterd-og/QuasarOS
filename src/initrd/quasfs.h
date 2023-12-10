#pragma once

#include <types.h>
#include <libc/string.h>

#define MAGIC 0xbadcafee

typedef struct {
    char magic[4];
    size_t fileCount;
} quasHead;

typedef struct {
    uint64_t magic; // should be 0xbadcafee
    char name[50];
    uint64_t address;
    size_t size;
} quasHdr;

int QuasFS_Init(void* addr);
char* QuasFS_Read(const char* name);
char** QuasFS_Dir();
size_t QuasFS_FTell(const char* name);
int QuasFS_GetEntryCount();