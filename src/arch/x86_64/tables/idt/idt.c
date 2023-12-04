#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/cpu/serial.h>
#include <arch/x86_64/cpu/pic.h>

__attribute__((aligned(0x10)))
static IDT_Entry IDT_Entries[256];
static IDTR     IDT_Data;
extern void*    IDT_IntTable[];

void*  IDT_Handlers[16] = {0};

static const char* IDT_Msgs[32] = {
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

void IDT_SetDesc(u8 vec, void* pIsr) {
    IDT_Entries[vec].low  = (u64)pIsr & 0xFFFF;
    IDT_Entries[vec].cs   = 0x28;
    IDT_Entries[vec].ist  = 0;
    IDT_Entries[vec].attr = (u8)0x8E;
    IDT_Entries[vec].mid  = ((u64)pIsr >> 16) & 0xFFFF;
    IDT_Entries[vec].high = ((u64)pIsr >> 32) & 0xFFFFFFFF;
    IDT_Entries[vec].resv = 0;
}

void IDT_Init() {
    for (u8 vec = 0; vec < 48; vec++) {
        IDT_SetDesc(vec, IDT_IntTable[vec]);
    }

    IDT_Data = (IDTR){
        .size   = (u16)sizeof(IDT_Entry) * 48 - 1,
        .offset = (u64)IDT_Entries
    };

    asm ("lidt %0" :: "m"(IDT_Data));
}

void IRQ_Register(u8 vec, void* pHandler) {
    IDT_Handlers[vec] = pHandler;
}

void IRQ_Unregister(u8 vec) {
    IDT_Handlers[vec] = 0;
}

void ISR_Handler(Registers* pRegs) {
    IRQ_Unregister(0); // Disable tasking
    
    asm volatile("cli");

    Serial_Printf("\nUh oh!\nSomething went wrong: %s\n", IDT_Msgs[pRegs->intNo]);

    for (;;)asm volatile("hlt");
}

void IRQ_Handler(Registers* pRegs) {
    void(*HandlerFunc)(Registers*) = IDT_Handlers[pRegs->intNo - 32];

    if ((u64)IDT_Handlers[pRegs->intNo - 32] != 0) {
        HandlerFunc(pRegs);
    }
    
    PIC_Eoi(pRegs->intNo - 32);
}