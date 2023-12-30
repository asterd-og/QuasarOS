#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/cpu/serial.h>
#include <arch/x86_64/cpu/pic.h>
#include <sched/sched.h>
#include <sched/ipc.h>
#include <mm/vmm.h>

__attribute__((aligned(0x10)))
static idt_entry idt_entries[256];
static idtr      idt_data;
extern void*     idt_int_table[];

void*  idt_handlers[17] = {0};

static const char* idt_msg[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

void idt_set_desc(u8 vec, void* pIsr) {
    idt_entries[vec].low  = (u64)pIsr & 0xFFFF;
    idt_entries[vec].cs   = 0x28;
    idt_entries[vec].ist  = 0;
    idt_entries[vec].attr = (u8)0x8E;
    idt_entries[vec].mid  = ((u64)pIsr >> 16) & 0xFFFF;
    idt_entries[vec].high = ((u64)pIsr >> 32) & 0xFFFFFFFF;
    idt_entries[vec].resv = 0;
}

void idt_init() {
    for (u8 vec = 0; vec < 49; vec++) {
        idt_set_desc(vec, idt_int_table[vec]);
    }

    idt_data = (idtr) {
        .size   = (u16)sizeof(idt_entry) * 49 - 1,
        .offset = (u64)idt_entries
    };

    __asm__ ("lidt %0" :: "m"(idt_data));
}

void irq_register(u8 vec, void* handler) {
    idt_handlers[vec] = handler;
}

void irq_unregister(u8 vec) {
    idt_handlers[vec] = 0;
}

u64 read_cr2() {
    u64 ret;
    __asm__ volatile("mov %%cr2, %0" :"=r"(ret) :: "memory");
    return ret;
}

void isr_handler(registers* regs) {
    sched_lock();
    if (regs->int_no == 14) {
        if (read_cr3() != (u64)to_physical(page_map_kernel)) {
            // Seg fault in an ELF
            if (!sched_is_dead()) {
                serial_printf("Segmentation fault!\n");
                ipc_transmit(SIGSEGV, 1);
                sched_kill();
            }
            sched_unlock();
            return;
        }
    }

    irq_unregister(0); // Disable tasking
    
    __asm__ volatile("cli");

    serial_printf("\nUh oh!\nSomething went wrong: %s\n", idt_msg[regs->int_no]);
    serial_printf("RSP: %lx | RIP: %lx | CR3: %lx | CR2: %lx\n", regs->rsp, regs->rip, read_cr3(), read_cr2());

    for (;;)__asm__ volatile("hlt");
}

void irq_handler(registers* regs) {
    void(*handler)(registers*);

    handler = idt_handlers[regs->int_no - 32];

    if ((u64)idt_handlers[regs->int_no - 32] != 0) {
            handler(regs);
    }
    
    pic_eoi(regs->int_no - 32);
}