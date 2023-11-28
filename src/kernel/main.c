#include <types.h>
#include <limine.h>
#include <arch/x86_64/tables/gdt/gdt.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/cpu/serial.h>
#include <flanterm/backends/fb.h>
#include <arch/x86_64/cpu/pic.h>
#include <arch/x86_64/mm/pmm.h>
#include <arch/x86_64/mm/vmm.h>
#include <flanterm/flanterm.h>
#include <kernel/kernel.h>
#include <libc/printf.h>
#include <heap/heap.h>

volatile struct limine_framebuffer_request fbReq = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

volatile struct limine_hhdm_request hhdmReq = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

struct flanterm_context *Flanterm_Context;
struct limine_framebuffer_response* Framebuffer_Data;

u64 HHDM_Offset;

static volatile struct limine_module_request modReq = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

struct limine_file* findModule(int pos) {
    return modReq.response->modules[pos];
}

void _start(void) {
    HHDM_Offset = hhdmReq.response->offset;
    Framebuffer_Data = fbReq.response;
    
    if (fbReq.response == NULL
     || fbReq.response->framebuffer_count < 1) {
        for (;;) asm("hlt");
    }

    GDT_Init();
    Serial_Init();

    Flanterm_Context = flanterm_fb_simple_init(
        Framebuffer_Data->framebuffers[0]->address,
        Framebuffer_Data->framebuffers[0]->width,
        Framebuffer_Data->framebuffers[0]->height,
        Framebuffer_Data->framebuffers[0]->pitch
    );

    asm volatile("cli");

    IDT_Init();
    PIC_Remap();

    asm volatile("sti");

    PMM_Init();

    printf("Loading VMM...\n");

    VMM_Init();

    printf("VMM Loaded.\n");

    Heap_Init((uptr)toHigherHalf(PMM_Alloc(1)));

    while (1) {
    }
}

void putchar_(char c) {
    char msg[] = {c, '\0'};
    flanterm_write(Flanterm_Context, msg, sizeof(msg));
}
