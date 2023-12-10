#include <sched/sched.h>
#include <arch/x86_64/cpu/serial.h>
#include <libc/lock.h>
#include <libc/printf.h>
#include <exec/elf/elf.h>

Sched_Task* Sched_TaskList[Sched_MaxTaskLimit] = {};
Sched_Task* Sched_CurrentTask = NULL;
u64 Sched_TID = 0;
u64 Sched_CTID = 0;
u64 Sched_Stage = 0;
u64 Sched_LockCounter = 0;
u64 Sched_TempTID = 0;

static Locker Sched_AtomicLock;

void Sched_IdleTask() {
    while(1) {
        asm ("hlt");
    }
}

void Sched_Init() {
    Sched_CreateNewTask(Sched_IdleTask);
}

void Sched_Wrapper(void* addr) {
    ((void(*)())addr)();
    Sched_CurrentTask->state = DEAD;
    while (1) {
        asm ("hlt");
        // We halt, so we wait for this task to be killed.
    }
}

Sched_Task* Sched_CreateNewTask(void* addr) {
    Sched_Lock();

    Sched_Task* task = (Sched_Task*)Heap_Alloc(sizeof(Sched_Task));
    task->TID = Sched_TID;

    char* stack = (char*)Heap_Alloc(0x4000);
    memset(stack, 0, 0x4000);

    u64* stackPtr = (u64*)(stack + 0x4000);

    task->regs.rip = (u64)Sched_Wrapper;
    task->regs.rdi = (u64)addr;
    task->regs.cs = 0x28;
    task->regs.ss = 0x10;
    task->regs.rflags = 0x202; // Interrupts enabled + necessary bit
    task->regs.rsp = (u64)stackPtr;

    task->pageMap = PageMap_New();

    task->state = RUNNING;

    Sched_TaskList[Sched_TID] = task;

    Sched_TID++;

    Sched_Unlock();

    return task;
}

Sched_Task* Sched_CreateNewElf(void* addr) {
    Sched_Lock();

    Sched_Task* task = (Sched_Task*)Heap_Alloc(sizeof(Sched_Task));
    task->TID = Sched_TID;

    char* stack = (char*)Heap_Alloc(0x4000);
    memset(stack, 0, 0x4000);

    u64* stackPtr = (u64*)(stack + 0x4000);

    task->pageMap = PageMap_New();

    task->regs.rip = (u64)Sched_Wrapper;
    task->regs.rdi = ELF_Exec(addr, task->pageMap);
    task->regs.cs = 0x28;
    task->regs.ss = 0x10;
    task->regs.rflags = 0x202; // Interrupts enabled + necessary bit
    task->regs.rsp = (u64)stackPtr;

    task->state = RUNNING;

    Sched_TaskList[Sched_TID] = task;

    Sched_TID++;

    Sched_Unlock();

    return task;
}

void Sched_KillTask(u64 TID) {
    Sched_Lock();
    if (TID > 0) {
        // IDLE is unkillable
        Sched_TaskList[TID]->state = DEAD;
    }
    Sched_Unlock();
}

void Sched_RemoveTask(u64 TID) {
    Sched_Lock();
    
    Heap_Free((void*)((u64*)(Sched_TaskList[TID]->regs.rsp)));
    Heap_Free(Sched_TaskList[TID]);

    // Re-arrange the task list to account for the dead task
    Sched_TaskList[TID] = NULL;

    if (TID != Sched_TID) {
        for (u64 i = TID; i < Sched_TID; i++) {
            Sched_TaskList[i] = Sched_TaskList[i + 1];
        }
    }

    Sched_TID--;

    Sched_Unlock();
}

u64 Sched_GetCurrentTID() {
    return Sched_CTID;
}

void Sched_Schedule(Registers* regs) {

    if (Sched_CurrentTask != NULL) {
        if (Sched_CurrentTask->state == DEAD) {
            Sched_RemoveTask(Sched_CurrentTask->TID);
            Sched_CurrentTask = NULL;
            Sched_CTID = 0;
            return;
        }

        Sched_CurrentTask->regs = *regs;
    }

    Sched_CurrentTask = Sched_TaskList[Sched_CTID];
    *regs = Sched_CurrentTask->regs;
    PageMap_Load(Sched_CurrentTask->pageMap);

    Sched_CTID++;
    if (Sched_CTID == Sched_TID) {
        Sched_CTID = 0;
    }

}

void Sched_Lock() {
    Lock(&Sched_AtomicLock);
}

void Sched_Unlock() {
    Unlock(&Sched_AtomicLock);
}