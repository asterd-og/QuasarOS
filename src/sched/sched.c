#include <sched/sched.h>
#include <arch/x86_64/cpu/serial.h>
#include <libc/lock.h>
#include <libc/printf.h>
#include <exec/elf/elf.h>
#include <initrd/quasfs.h>

Sched_Task* Sched_TaskList[Sched_MaxTaskLimit] = {};
Sched_Task* Sched_CurrentTask = NULL;
u64 Sched_TID = 0;
u64 Sched_CTID = 0;
u64 Sched_Stage = 0;
u64 Sched_LockCounter = 0;
u64 Sched_TempTID = 0;
bool Sched_Started = 0;
u64 Sched_Carol = 0;

bool Sched_Paused = 0;

static Locker Sched_AtomicLock;

void Sched_IdleTask() {
    while(1) {
        asm ("hlt");
    }
}

void Sched_Init() {
    Sched_Task* idle = Sched_CreateNewTask(Sched_IdleTask, "Idle", false);
    Sched_QueueTask(idle);
    Sched_Started = true;
}

void Sched_Wrapper(void* addr) {
    ((void(*)())addr)();
    Sched_CurrentTask->state = DEAD;
    while (1) {
        asm ("hlt");
        // We halt, so we wait for this task to be killed.
    }
}

u64 Sched_GetBootTime() {
    u64 ret;
    asm volatile("rdtsc" : "=a"(ret));
    return ret;
}

void Sched_QueueTask(Sched_Task* task) {
    Sched_Lock();
    Sched_Paused = 1;

    Sched_TaskList[Sched_TID] = task;
    Sched_TID++;

    Sched_Paused = 0;
    Sched_Unlock();
}

Sched_Task* Sched_CreateNewTask(void* addr, char* name, bool killable) {
    Sched_Lock();

    Sched_Task* task = (Sched_Task*)Heap_Alloc(sizeof(Sched_Task));
    task->TID = Sched_TID;
    task->name = name;
    task->killable = killable;

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

    task->startTime = Sched_GetBootTime();

    Sched_Unlock();

    return task;
}

Sched_Task* Sched_CreateNewElf(char* addr, char* name, bool killable) {
    Sched_Lock();
    
    Sched_Task* task = (Sched_Task*)Heap_VAlloc(PageMap_Kernel, sizeof(Sched_Task));
    task->TID = Sched_TID;
    task->name = name;
    task->killable = killable;

    task->pageMap = PageMap_New();

    char* stack = (char*)Heap_VAlloc(task->pageMap, 0x4000);
    memset(stack, 0, 0x4000);

    u64* stackPtr = (u64*)(stack + 0x4000);

    task->regs.rip = (u64)Sched_Wrapper;
    task->regs.rdi = ELF_Exec(addr, task->pageMap);
    task->regs.cs = 0x28;
    task->regs.ss = 0x10;
    task->regs.rflags = 0x202; // Interrupts enabled + necessary bit
    task->regs.rsp = (u64)stackPtr;

    task->state = RUNNING;
    
    task->startTime = Sched_GetBootTime();

    Sched_Unlock();

    return task;
}

void Sched_KillTask(u64 TID) {
    Sched_Lock();
    if (Sched_TaskList[TID]->killable) {
        // IDLE is unkillable
        printf("killing task %ld\n", TID);
        Sched_TaskList[TID]->state = DEAD;
    }
    Sched_Unlock();
}

void Sched_RemoveTask(u64 TID) {
    Sched_Lock();
    
    PageMap_Delete(Sched_TaskList[TID]->pageMap);
    Heap_Free((void*)((u64*)(Sched_TaskList[TID]->regs.rsp - 0x4000)));
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
    if (Sched_Paused) {
        while (Sched_Paused) asm("nop");
        return;
    }

    if (Sched_CurrentTask != NULL) {
        if (Sched_CurrentTask->state == DEAD) {
            Sched_RemoveTask(Sched_CurrentTask->TID);
            Sched_CurrentTask = NULL;
            Sched_CTID = 0;
            return;
        }

        Sched_CurrentTask->usage = Sched_GetBootTime() - Sched_CurrentTask->startTime;
        Sched_CurrentTask->regs = *regs;
    }

    Sched_CurrentTask = Sched_TaskList[Sched_CTID];

    if (Sched_CurrentTask->state == RUNNING) {
        PageMap_Load(Sched_CurrentTask->pageMap);
        *regs = Sched_CurrentTask->regs;
        Sched_CurrentTask->startTime = Sched_GetBootTime();
    }

    Sched_CTID++;
    if (Sched_CTID == Sched_TID) {
        Sched_CTID = 0;
    }
}

void Sched_Lock() {
    Sched_Paused = true;
    Lock(&Sched_AtomicLock);
}

void Sched_Unlock() {
    Unlock(&Sched_AtomicLock);
    Sched_Paused = false;
}