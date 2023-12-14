#include <sched/sched.h>
#include <arch/x86_64/cpu/serial.h>
#include <libc/lock.h>
#include <libc/printf.h>
#include <exec/elf/elf.h>
#include <initrd/quasfs.h>

sched_task* sched_task_list[sched_max_task_limit] = {};
sched_task* sched_current_task = NULL;
u64 sched_tid = 0;
u64 sched_ctid = 0;
u64 initial_time = 0;

Locker sched_atomic_lock;

void sched_idle_task() {
    while(1) {
        asm ("hlt");
    }
}

u64 sched_get_boot_time() {
    u64 ret;
    asm volatile("rdtsc" : "=a"(ret));
    return ret / 1000000;
}

void sched_init() {
    initial_time = sched_get_boot_time();
    sched_task* idle = sched_create_new_task(sched_idle_task, "Idle", false);
    sched_queue_task(idle);
}

void sched_wrapper(void* addr) {
    ((void(*)())addr)();
    sched_current_task->state = DEAD;
    while (1) {
        asm ("hlt");
        // We halt, so we wait for this task to be killed.
    }
}

void sched_queue_task(sched_task* task) {
    sched_lock();

    sched_task_list[sched_tid] = task;
    sched_tid++;

    sched_unlock();
}

sched_task* sched_create_new_task(void* addr, char* name, bool killable) {
    sched_lock();

    sched_task* task = (sched_task*)kmalloc(sizeof(sched_task));
    task->TID = sched_tid;
    task->name = name;
    task->killable = killable;

    char* stack = (char*)kmalloc(0x4000);
    memset(stack, 0, 0x4000);

    u64* stack_ptr = (u64*)(stack + 0x4000);

    task->regs.rip = (u64)sched_wrapper;
    task->regs.rdi = (u64)addr;
    task->regs.cs = 0x28;
    task->regs.ss = 0x10;
    task->regs.rflags = 0x202; // Interrupts enabled + necessary bit
    task->regs.rsp = (u64)stack_ptr;

    task->page_map = page_map_new();

    task->state = RUNNING;

    task->start_time = sched_get_boot_time();

    sched_unlock();

    return task;
}

sched_task* sched_create_new_elf(char* addr, char* name, bool killable) {
    sched_lock();
    
    sched_task* task = (sched_task*)kmalloc(sizeof(sched_task));
    task->TID = sched_tid;
    task->killable = killable;
    task->name = name;

    task->page_map = page_map_new();

    char* stack = (char*)vmm_alloc(task->page_map, div_round_up(0x4000 / page_size, page_size), vmm_flag_present | vmm_flag_write);
    memset(stack, 0, 0x4000);

    u64* stack_ptr = (u64*)(stack + 0x4000);

    task->regs.rip = (u64)sched_wrapper;
    task->regs.rdi = elf_exec(addr, task->page_map);
    task->regs.cs = 0x28;
    task->regs.ss = 0x10;
    task->regs.rflags = 0x202; // Interrupts enabled + necessary bit
    task->regs.rsp = (u64)stack_ptr;

    task->state = RUNNING;
    
    task->start_time = sched_get_boot_time();

    sched_unlock();

    return task;
}

void sched_kill_task(u64 TID) {
    sched_lock();
    if (sched_task_list[TID]->killable) {
        // IDLE is unkillable
        printf("killing task %ld\n", TID);
        sched_task_list[TID]->state = DEAD;
    }
    sched_unlock();
}

void sched_remove_task(u64 TID) {
    sched_lock();
    
    page_map_delete(sched_task_list[TID]->page_map);
    kfree((void*)((u64*)(sched_task_list[TID]->regs.rsp - 0x4000)));
    kfree(sched_task_list[TID]);

    // Re-arrange the task list to account for the dead task
    sched_task_list[TID] = NULL;

    if (TID != sched_tid) {
        for (u64 i = TID; i < sched_tid; i++) {
            sched_task_list[i] = sched_task_list[i + 1];
        }
    }

    sched_tid--;

    sched_unlock();
}

u64 sched_get_current_tid() {
    return sched_ctid;
}

u64 sched_get_tid() {
    return sched_tid;
}

u64 sched_task_get_usage(u64 tid) {
    return sched_task_list[tid]->usage;
}

char* sched_task_get_name(u64 tid) {
    return sched_task_list[tid]->name;
}

void sched_switch(registers* regs) {
    if (sched_current_task != NULL) {
        if (sched_current_task->state == DEAD) {
            sched_remove_task(sched_current_task->TID);
            sched_current_task = NULL;
            sched_ctid = 0;
            return;
        }

        sched_current_task->regs = *regs;
    }

    sched_current_task = sched_task_list[sched_ctid];

    if (sched_current_task->state == RUNNING) {
        page_map_load(sched_current_task->page_map);
        *regs = sched_current_task->regs;
    }

    sched_ctid++;
    if (sched_ctid == sched_tid) {
        sched_ctid = 0;
    }
}

void sched_lock() {
    lock(&sched_atomic_lock);
}

void sched_unlock() {
    unlock(&sched_atomic_lock);
}
