#include <arch/x86_64/smp/apic/lapic.h>

void lapic_write(u32 reg, u32 val) {
    *((volatile u32*)((uptr)LAPIC_BASE + reg)) = val;
}

u32 lapic_read(u32 reg) {
    return *((volatile u32*)((uptr)LAPIC_BASE + reg));
}

void lapic_init() {
    lapic_write(LAPIC_SPURIOUS, lapic_read(LAPIC_SPURIOUS) | (1 << 8) | 0xff);
}