#include <arch/x86_64/cpu/pit.h>
#include <sched/sched.h>

u64 pit_tick = 0;

void pit_sleep(u64 ms) {
    u64 start = pit_tick;
    while (pit_tick < start + ms * (pit_base_freq / 100));
}

void pit_handler(registers* regs) {
    sched_switch(regs);
}

void pit_init() {
    outb(0x43, 0x36);
    u64 div = 1193180 / pit_base_freq;
    outb(0x40, (u8)div);
    outb(0x40, (u8)(div >> 8));
    irq_register(0, pit_handler);
}