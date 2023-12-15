#include <sched/sched.h>
#include <arch/x86_64/cpu/serial.h>
#include <libc/lock.h>
#include <libc/printf.h>
#include <exec/elf/elf.h>
#include <initrd/quasfs.h>

sched_task* sched_list[SCHED_MAX_TASK];
sched_task* sched_current_task = NULL;
u64 sched_tid = 0;
u64 sched_cid = 0;

Locker sched_atomic_lock;
bool running = false;

void sched_init() {
    running = true;
}

void sched_wrapper(void* addr) {
    ((void(*)())addr)();
    sched_list[sched_cid]->state = DEAD;
    while (1) {
        asm ("hlt");
        // We halt, so we wait for this task to be killed.
    }
}

void sched_create_new_task(void* addr, char* name, bool killable, bool elf) {
    page_map_load(page_map_kernel);

    sched_task* task = (sched_task*)kmalloc(sizeof(sched_task));

    task->name = name;
    task->page_map = page_map_new();
    serial_printf("%s's page map is: 0x%lx\n", name, to_physical(task->page_map));
    task->id = sched_tid;
    task->state = RUNNING;
    task->killable = killable;
    sched_list[sched_tid] = task;
    
    sched_tid++;

    // Set up registers.
    
    // TODO: Implement wrapper, to safely exit tasks.
    task->regs.rip = (elf ? elf_exec(addr, task->page_map) : (u64)addr);
    task->regs.cs = 0x28;
    task->regs.ss = 0x30;
    task->regs.rflags = 0x202; // Ints enabled + necessary bit.
    
    // Create stack

    char* stack = (char*)vmm_alloc(task->page_map, align_up(STACK_SIZE, page_size) / page_size, vmm_flag_present | vmm_flag_write);
    page_map_load(task->page_map);
    memset(stack, 0, STACK_SIZE);
    page_map_load(page_map_kernel);

    task->regs.rsp = (u64)(stack + STACK_SIZE); // RSP has to be the stack top.

    serial_printf("Created task '%s'\n", name);
}

void sched_switch(registers* regs) {
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
}

void sched_lock() {
    lock(&sched_atomic_lock);
}

void sched_unlock() {
    unlock(&sched_atomic_lock);
}