#include <sched/sched.h>
#include <arch/x86_64/cpu/serial.h>
#include <libc/lock.h>
#include <libc/printf.h>
#include <exec/elf/elf.h>
#include <initrd/quasfs.h>

sched_task* sched_list[SCHED_MAX_TASK];
sched_task* sched_current_task;
u64 sched_tid = 0;
u64 sched_cid = 0;

Locker sched_atomic_lock;
bool running = false;

void sched_init() {
    running = true;
}

void sched_wrapper(void* addr) {
    ((void(*)())addr)();
    sched_current_task->state = DEAD;
    while (1) {
        asm ("hlt");
        // We halt, so we wait for this task to be killed.
    }
}

void sched_create_new_task(void* addr, char* name, bool killable, bool elf) {
    sched_lock();
    sched_task* task = (sched_task*)kmalloc(sizeof(sched_task));

    task->name = name;
    //task->page_map = (elf ? page_map_new() : (page_map*)to_higher_half(read_cr3()));
    task->page_map = page_map_new();
    task->id = sched_tid;
    task->state = RUNNING;
    task->killable = killable;
    
    // Set up registers.
    
    task->regs.rip = (u64)sched_wrapper;
    task->regs.rdi = (elf ? elf_exec(addr, task->page_map) : (u64)addr);
    task->regs.cs = 0x28;
    task->regs.ss = 0x30;
    task->regs.rflags = 0x202; // Ints enabled + necessary bit.
    
    // Create stack

    char* stack = (char*)to_higher_half(pmm_alloc(1));

    task->regs.rsp = (u64)(stack + STACK_SIZE); // RSP has to be the stack top.

    serial_printf("Created task '%s' stack at %lx\n", name, task->regs.rsp);
    sched_list[sched_tid] = task;
    sched_tid++;

    sched_unlock();
}

void sched_switch(registers* regs) {
    sched_lock();
    if (sched_current_task) {
        sched_current_task->regs = *regs;
    }

    sched_current_task = sched_list[sched_cid];

    *regs = sched_current_task->regs;
    page_map_load(sched_current_task->page_map);

    sched_cid++;
    if (sched_cid == sched_tid) {
        sched_cid = 0;
    }
    sched_unlock();
}

void sched_lock() {
    lock(&sched_atomic_lock);
}

void sched_unlock() {
    unlock(&sched_atomic_lock);
}