#include <exec/syscall.h>
#include <initrd/quasfs.h>
#include <sched/sched.h>
#include <drivers/kb.h>

/*
Syscalls spec:
    rax - function
    rbx - first arg
    rcx - second arg
    rdx - third arg
*/

void syscall_handler(registers* regs) {
    sched_lock();
    switch (regs->rax) {
        case 0x00:
            // puts
            printf("%s", (char*)regs->rbx);
            break;
        case 0x01:
            // putf
            printf((char*)regs->rbx, regs->rcx);
            break;
        case 0x02:
            // KB Get char
            regs->rax = kb_get_char();
            break;
        case 0x03:
            // List dir initrd
            regs->rax = (u64)quasfs_dir();
            break;
        case 0x04:
            // Get entry count
            regs->rax = quasfs_get_entry_count();
            break;
        case 0x05:
            // Read file
            regs->rax = (u64)quasfs_read((char*)regs->rbx);
            break;
        case 0x06:
            // Start new elf task
            sched_create_new_elf(quasfs_read((char*)regs->rbx), (char*)regs->rbx, true);
            break;
        case 0x07:
            // Get sched tid
            regs->rax = sched_get_tid();
            break;
        case 0x08:
            // Get task usage
            regs->rax = sched_task_get_usage(regs->rbx);
            break;
        case 0x09:
            // Get task name
            regs->rax = (u64)sched_task_get_name(regs->rbx);
            break;
    }
    sched_unlock();
}

void syscall_init() {
    irq_register(16, syscall_handler);
}