#pragma once

#include <types.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <mm/vmm.h>
#include <heap/heap.h>

#define STACK_SIZE 0x4000
#define SCHED_MAX_TASK 16

enum {
    WAITING,
    RUNNING,
    BUSY,
    DEAD
};

typedef struct {
    char* name;
    page_map* page_map;
    int argc;
    char** argv;
    registers regs;
    u64 id;
    u8 state;
    bool killable;
} sched_task;

void sched_init();

u64 sched_create_new_task(void* addr, char* name, bool killable, bool elf, char** args, int argc);
u64 sched_get_pid();

void sched_switch(registers* regs);

void sched_lock();
void sched_unlock();