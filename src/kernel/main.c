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
#include <kernel/kernel.h>
#include <drivers/mouse.h>
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

void IdleTask() {
    while(1) {
        asm volatile("hlt");
    }
}

void VBE_Task() {
    while(1) {
        FB_SetPix(VBE, Mouse_X, Mouse_Y, 0xFF000000);
        FB_SetPix(VBE, Mouse_X+1, Mouse_Y, 0xFF000000);
        FB_SetPix(VBE, Mouse_X+1, Mouse_Y+1, 0xFF000000);
        FB_SetPix(VBE, Mouse_X, Mouse_Y+1, 0xFF000000);
        VBE_Update();
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

    PMM_Init();

    Serial_Printf("Loading VMM.\n");

    VMM_Init();

    Serial_Printf("VMM Loaded.\n");

    Heap_Init((uptr)toHigherHalf(PMM_Alloc(1)));

    VBE_Init();
    Mouse_Init();

    Flanterm_FB = FB_CreateNewFB(
        200, 200, 500, 500, VBE->pitch
    );

    Flanterm_Context = flanterm_fb_simple_init(
        Flanterm_FB->buffer,
        Flanterm_FB->width, Flanterm_FB->height,
        VBE->pitch
    );

    printf("Hey!\n");
    FB_Clear(VBE, 0xFFFF00FF);
    FB_CopyFB(Flanterm_FB, VBE);

    Sched_CreateNewTask(VBE_Task);

    PIT_Init();

    for (;;);
}

Locker PutCh_Lock;

void putchar_(char c) {
    char msg[] = {c, '\0'};
    flanterm_write(Flanterm_Context, msg, sizeof(msg));
}