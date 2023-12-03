#include <arch/x86_64/cpu/pit.h>
#include <sched/sched.h>

void PIT_Handler(Registers* regs) {
    Sched_SwitchTask(regs);
}

void PIT_Init() {
    outb(0x43, 0x36);
    u64 div = 1193180 / 100;
    outb(0x40, (u8)div);
    outb(0x40, (u8)(div >> 8));
    IRQ_Register(0, PIT_Handler);
}