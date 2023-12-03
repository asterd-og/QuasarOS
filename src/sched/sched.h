#pragma once

#include <types.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <heap/heap.h>

#define Sched_MaxTaskLimit 16

typedef struct {
    Registers regs;
    u64 TID; // Task ID
} Sched_Task;

Sched_Task* Sched_CreateNewTask(void* addr);
void Sched_SwitchTask(Registers* regs);