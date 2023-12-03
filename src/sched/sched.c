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
    Sched_Task* task = (Sched_Task*)Heap_Alloc(sizeof(Sched_Task));
    task->TID = Sched_TID;

    task->regs.rip = (u64)addr;
    task->regs.rflags = 0x202; // Interrupts enabled + necessary bit
    task->regs.rsp = (u64)Heap_Alloc(4096);

    Sched_TaskList[Sched_TID] = task;

    Sched_TID++;

    return task;
}

void Sched_SwitchTask(Registers* regs) {
    Lock(&Sched_Lock);

    if (Sched_CTID == Sched_TID) {
        Sched_CTID = 0;
    }

    Sched_Task* task = Sched_TaskList[Sched_CTID];
    regs->r15 = task->regs.r15;
    regs->r14 = task->regs.r14;
    regs->r13 = task->regs.r13;
    regs->r12 = task->regs.r12;
    regs->r11 = task->regs.r11;
    regs->r10 = task->regs.r10;
    regs->r9 = task->regs.r9;
    regs->r8 = task->regs.r8;
    regs->rdi = task->regs.rdi;
    regs->rsi = task->regs.rsi;
    regs->rbp = task->regs.rbp;
    regs->rbx = task->regs.rbx;
    regs->rdx = task->regs.rdx;
    regs->rcx = task->regs.rcx;
    regs->rax = task->regs.rax;
    regs->rflags = task->regs.rflags;
    regs->rsp = task->regs.rsp;
    regs->rip = task->regs.rip;

    Sched_CTID++;

    Unlock(&Sched_Lock);
}