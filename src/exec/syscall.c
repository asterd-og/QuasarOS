#include <exec/syscall.h>
#include <initrd/quasfs.h>
#include <sched/sched.h>
#include <drivers/kb.h>
#include <arch/x86_64/cpu/pit.h>
#include <sched/ipc.h>
#include <drivers/rtc.h>
#include <fs/file.h>

/*
Syscalls spec:
    rax - function
    rbx - first arg
    rcx - second arg
    rdx - third arg
    rsi - fourth arg
    rdi - fifth arg
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
            regs->rax = (u64)file_read((char*)regs->rbx, (char*)regs->rcx);
            break;
        case 0x06:
            // Start new elf task, it's PID will be on RAX
            regs->rax = sched_create_new_task(quasfs_read((char*)regs->rbx), (char*)regs->rbx, true, true, (char**)regs->rcx, regs->rdx);
            break;
        case 0x07:
            // Ipc get (u64 pid)
            regs->rax = ipc_get(regs->rbx);
            break;
        case 0x08:
            // Ipc get ret (u64 pid)
            regs->rax = ipc_get_ret(regs->rbx);
            break;
        case 0x09:
            // Ipc dispatch (u64 pid)
            ipc_dispatch(regs->rbx);
            break;
        case 0x0a:
            // Kmalloc
            regs->rax = (u64)kmalloc(regs->rbx);
            break;
        case 0x0b:
            // Kfree
            kfree((void*)regs->rbx);
            break;
        case 0x0c:
            // Malloc
            regs->rax = (u64)malloc(regs->rbx);
            break;
        case 0x0d:
            // Free
            free((void*)regs->rbx);
            break;
        case 0x0e:
            // Clear
            flanterm_context->clear(flanterm_context, true);
            break;
        case 0x0f:
            // Set fg
            flanterm_context->set_text_fg_rgb(flanterm_context, regs->rbx);
            break;
        case 0x10:
            // Set bg
            flanterm_context->set_text_bg_rgb(flanterm_context, regs->rbx);
            break;
        case 0x11:
            // Reset fg
            flanterm_context->set_text_fg_default(flanterm_context);
            break;
        case 0x12:
            // Reset bg
            flanterm_context->set_text_bg_default(flanterm_context);
            break;
        case 0x13:
            // Ftell
            regs->rax = quasfs_ftell((char*)regs->rbx);
            break;
        case 0x14:
            // Sleep
            rtc_sleep(regs->rbx);
            break;
    }
    sched_unlock();
}

void syscall_init() {
    irq_register(16, syscall_handler);
}
