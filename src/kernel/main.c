#include <types.h>
#include <limine.h>
#include <arch/x86_64/tables/gdt/gdt.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/smp/acpi/acpi.h>
#include <arch/x86_64/smp/apic/lapic.h>
#include <arch/x86_64/cpu/serial.h>
#include <flanterm/backends/fb.h>
#include <arch/x86_64/cpu/pic.h>
#include <arch/x86_64/cpu/pit.h>
#include <arch/x86_64/smp/smp.h>
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
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <wm/wm.h>

volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

struct flanterm_context *flanterm_context;

u64 HHDM_Offset;

static volatile struct limine_module_request modReq = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

struct limine_file* find_module(int pos) {
    return modReq.response->modules[pos];
}

void kernel_idle() {
    while (1) {
        asm ("hlt");
    }
}

bool sse_enabled = false;

void _start(void) {
    HHDM_Offset = hhdm_request.response->offset;

    gdt_init();
    serial_init();

    asm ("cli");
    idt_init();
    pic_remap();
    asm ("sti");

    pmm_init();
    vmm_init();

    quasfs_init(find_module(0)->address);

    vbe_init();

    /*
    
    Enable SSE

    */

    u64 cr0;
    u64 cr4;
    asm volatile("mov %%cr0, %0" :"=r"(cr0) :: "memory");
    cr0 &= ~((u64)1 << 2);
    cr0 |= (u64)(1 << 1);
    asm volatile("mov %0, %%cr0" :: "r"(cr0) : "memory");
    asm volatile("mov %%cr4, %0" :"=r"(cr4) :: "memory");
    cr4 |= (u64)(3 << 9);
    asm volatile("mov %0, %%cr4" :: "r"(cr4) : "memory");
    sse_enabled = true;

    syscall_init();
    mouse_init();
    kb_init();
    wm_init();

    wm_window* terminal = wm_create_new_window("Terminal", 640, 400);

    flanterm_context = flanterm_fb_simple_init(
        terminal->fb->buffer,
        (size_t)terminal->fb->width, (size_t)terminal->fb->height,
        (size_t)terminal->fb->pitch
    );

    acpi_init();
    lapic_init();
    smp_init_all();

    char* shell = quasfs_read("shell");

    sched_create_new_task(kernel_idle, "idle", false, false, (char**){0}, 0);
    sched_create_new_task(shell, "shell", true, true, (char**){0}, 0);
    sched_init();
    pit_init();

    while(1) {
        asm ("hlt");
    }
}

void putchar_(char c) {
    char msg[] = {c, '\0'};
    flanterm_write(flanterm_context, msg, sizeof(msg));
}
