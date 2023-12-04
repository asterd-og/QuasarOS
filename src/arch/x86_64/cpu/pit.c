#include <arch/x86_64/cpu/pit.h>
#include <sched/sched.h>

u64 PIT_Tick = 0;
u64 Sched_Tick = 0;

void PIT_Sleep(u64 ms) {
    u64 start = PIT_Tick;
    while (PIT_Tick < start + ms * (PIT_BaseFreq / 100));
}

void PIT_Handler(Registers* regs) {
    PIT_Tick++;
    Sched_Tick++;
    if (Sched_Tick == 10 * (PIT_BaseFreq / 100)) {
        Sched_Schedule(regs);
        Sched_Tick = 0;
    }
}

void PIT_Init() {
    outb(0x43, 0x36);
    u64 div = 1193180 / PIT_BaseFreq;
    outb(0x40, (u8)div);
    outb(0x40, (u8)(div >> 8));
    IRQ_Register(0, PIT_Handler);
}