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

void Syscall_Handler(Registers* regs) {
    Sched_Lock();
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
            regs->rax = KB_GetChar();
            break;
        case 0x03:
            // List dir initrd
            regs->rax = (u64)QuasFS_Dir();
            break;
        case 0x04:
            // Get entry count
            regs->rax = QuasFS_GetEntryCount();
            break;
        case 0x05:
            // Read file
            regs->rax = (u64)QuasFS_Read((char*)regs->rbx);
            break;
        case 0x06:
            // Start new elf task
            Sched_CreateNewElf(QuasFS_Read((char*)regs->rbx));
            break;
        case 0x07:
            // Exit syscall
            Sched_KillTask(Sched_GetCurrentTID());
            break;
    }
    Sched_Unlock();
}

void Syscall_Init() {
    IRQ_Register(16, Syscall_Handler);
}