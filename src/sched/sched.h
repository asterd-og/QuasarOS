#pragma once

#include <types.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/mm/vmm.h>
#include <heap/heap.h>

#define Sched_MaxTaskLimit 16

enum {
    STARTING,
    RUNNING,
    DEAD
};

typedef struct {
    char* name;
    Registers regs;
    u64 TID; // Task ID
    u8 state;
    VMM_PageMap* pageMap;
    u64 startTime;
    u64 usage;
    bool killable;
} __attribute__((packed)) Sched_Task;

extern bool Sched_Paused;

void Sched_Init();

Sched_Task* Sched_CreateNewTask(void* addr, char* name, bool killable);
Sched_Task* Sched_CreateNewElf(char* addr, char* name, bool killable);

void Sched_QueueTask(Sched_Task* task);

void Sched_KillTask(u64 TID);
void Sched_RemoveTask(u64 TID);
u64 Sched_GetCurrentTID();
void Sched_Schedule(Registers* regs);

void Sched_Lock();
void Sched_Unlock();