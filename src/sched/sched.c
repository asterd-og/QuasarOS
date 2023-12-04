#include <sched/sched.h>
#include <arch/x86_64/cpu/serial.h>
#include <libc/lock.h>

Sched_Task* Sched_TaskList[Sched_MaxTaskLimit] = {};
Sched_Task* Sched_CurrentTask = NULL;
u64 Sched_TID = 0;
u64 Sched_CTID = 0;
u64 Sched_Stage = 0;
u64 Sched_LockCounter = 0;

static Locker Sched_AtomicLock;

Sched_Task* Sched_CreateNewTask(void* addr) {
    Sched_Lock();

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

    Sched_Unlock();

    return task;
}

void Sched_Schedule(Registers* regs) {
    if (Sched_CurrentTask != NULL) {
        memcpy(&Sched_CurrentTask->regs, regs, sizeof(Registers));
        Sched_CurrentTask = Sched_TaskList[Sched_CTID];
        memcpy(regs, &Sched_CurrentTask->regs, sizeof(Registers));
        Sched_CTID++;
        if (Sched_CTID == Sched_TID) {
            Sched_CTID = 0;
        }
    } else {
        Sched_CurrentTask = Sched_TaskList[Sched_CTID];
        memcpy(regs, &Sched_CurrentTask->regs, sizeof(Registers));
    }
}

void Sched_Lock() {
    Lock(&Sched_AtomicLock);
}

void Sched_Unlock() {
    Unlock(&Sched_AtomicLock);
}