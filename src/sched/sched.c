#include <sched/sched.h>
#include <libc/printf.h>
#include <libc/lock.h>
#include <arch/x86_64/cpu/serial.h>

Sched_Task* Sched_TaskList[Sched_MaxTaskLimit] = {};
u64 Sched_TID = 0;
u64 Sched_CTID = 0;
u64 stage = 0;

Locker Sched_Lock;

Sched_Task* Sched_CreateNewTask(void* addr) {
    Lock(&Sched_Lock);

    Sched_Task* task = (Sched_Task*)Heap_Alloc(sizeof(Sched_Task));
    task->TID = Sched_TID;

    char* stack = (char*)Heap_Alloc(0x4000);
    memset(stack, 0, 0x4000);

    u64* stackPtr = (u64*)(stack + 0x4000);
    *--stackPtr = 0; // Default return val

    task->regs.rip = (u64)addr;
    task->regs.rdi = (u64)addr;
    task->regs.cs = 0x28;
    task->regs.ss = 0x10;
    task->regs.rflags = 0x202; // Interrupts enabled + necessary bit
    task->regs.rsp = (u64)stackPtr;

    Sched_TaskList[Sched_TID] = task;

    Sched_TID++;

    Unlock(&Sched_Lock);

    return task;
}

void Sched_CopyContext(Registers* from, Registers* to) {
    to->r15 = from->r15;
    to->r14 = from->r14;
    to->r13 = from->r13;
    to->r12 = from->r12;
    to->r11 = from->r11;
    to->r10 = from->r10;
    to->r9 = from->r9;
    to->r8 = from->r8;
    to->rdi = from->rdi;
    to->rsi = from->rsi;
    to->rbp = from->rbp;
    to->rbx = from->rbx;
    to->rdx = from->rdx;
    to->rcx = from->rcx;
    to->rax = from->rax;
    to->rflags = from->rflags;
    to->rsp = from->rsp;
    to->rip = from->rip;
}

void Sched_SwitchTask(Registers* regs) {
    Lock(&Sched_Lock);
    
    if (stage == 0) {
        Sched_Task* task = Sched_TaskList[Sched_CTID];
        memcpy(regs, &task->regs, sizeof(Registers));

        stage = 1;
    } else {
        Sched_Task* task = Sched_TaskList[Sched_CTID];
        memcpy(&task->regs, regs, sizeof(Registers));

        Sched_CTID++;
        if (Sched_CTID == Sched_TID) {
            Sched_CTID = 0;
        }
        stage = 0;
    }

    Unlock(&Sched_Lock);
}