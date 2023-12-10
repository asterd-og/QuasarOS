#pragma once

#include <types.h>
#include <arch/x86_64/mm/vmm.h>
#include <heap/heap.h>
#include <libc/printf.h>

typedef struct {
    char magic[4];
    u8 bits;
    u8 endian;
    u8 version;
    u8 abi;
    u64 padding;
    u16 type;
    u16 isa;
    u32 ELFVersion;
    u64 entry;
    u64 phoff;
    u64 shoff;
    u32 flags;
    u16 hdrSize;
    u16 entryPhSize;
    u16 entryPhCount;
    u16 entryShSize;
    u16 entryShCount;
    u16 ShNames;
} ELF_Header;

typedef struct {
    u32 type;
    u32 flags;
    u64 offset;
    u64 vaddr;
    u64 paddr;
    u64 fileSize;
    u64 memSize;
    u64 align;
} ELF_ProgramHeader;

typedef struct {
    u32 name;
    u32 type;
    u64 flags;
    u64 addr;
    u64 offset;
    u64 size;
    u32 link;
    u32 info;
    u64 align;
    u64 shSize;
} ELF_SectionHeader;

#define ELF_PHDRLoad 1

#define ELF_FlagX 0x1
#define ELF_FlagW 0x2
#define ELF_FlagR 0x4

u64 ELF_Exec(char* addr, VMM_PageMap* pageMap);