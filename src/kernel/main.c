#include <types.h>
#include <limine.h>
#include <arch/x86_64/tables/gdt/gdt.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/cpu/serial.h>
#include <flanterm/backends/fb.h>
#include <arch/x86_64/cpu/pic.h>
#include <arch/x86_64/cpu/pit.h>
#include <arch/x86_64/mm/pmm.h>
#include <arch/x86_64/mm/vmm.h>
#include <flanterm/flanterm.h>
#include <exec/flat/flat.h>
#include <kernel/kernel.h>
#include <drivers/mouse.h>
#include <initrd/quasfs.h>
#include <exec/syscall.h>
#include <libc/printf.h>
#include <sched/sched.h>
#include <drivers/kb.h>
#include <video/vbe.h>
#include <heap/heap.h>
#include <video/fb.h>

volatile struct limine_hhdm_request hhdmReq = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

struct flanterm_context *Flanterm_Context;

u64 HHDM_Offset;

static volatile struct limine_module_request modReq = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

struct limine_file* findModule(int pos) {
    return modReq.response->modules[pos];
}

Framebuffer* Flanterm_FB;

void task1() {
    while(1) {
        printf("a");
    }
}

void task2() {
    while(1) {
        printf("b");
    }
}

void task3() {
    while(1) {
        printf("c");
    }
}

void task4() {
    while(1) {
        printf("d");
    }
}

void _start(void) {
    HHDM_Offset = hhdmReq.response->offset;

    GDT_Init();
    Serial_Init();

    asm ("cli");

    IDT_Init();
    PIC_Remap();

    asm ("sti");
    KB_Init();

    PMM_Init();
    VMM_Init();

    Heap_Init((uptr)toHigherHalf(PMM_Alloc(1)));

    VBE_Init();
    Syscall_Init();

    Flanterm_FB = FB_CreateNewFB(
        200, 200, 500, 500, VBE->pitch
    );

    Flanterm_Context = flanterm_fb_simple_init(
        VBE_GetAddr(),
        VBE->width, VBE->height,
        VBE->pitch
    );

    QuasFS_Init(findModule(0)->address);

    char* addr = QuasFS_Read("shell");
    if (addr == NULL) {
        printf("dang");
    }

    Sched_Init();
    Sched_QueueTask(Sched_CreateNewElf(addr, "shell", true));
    PIT_Init();

    for (;;);
}

void putchar_(char c) {
    char msg[] = {c, '\0'};
    flanterm_write(Flanterm_Context, msg, sizeof(msg));
}