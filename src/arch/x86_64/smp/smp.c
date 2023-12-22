#include <arch/x86_64/smp/smp.h>
#include <arch/x86_64/cpu/pit.h>
#include <libc/printf.h>
#include <video/vbe.h>
#include <mm/vmm.h>
#include <wm/wm.h>

volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

// Thank you KM198912 again! (https://github.com/KM198912)

static int frames = 0;
static int last_tick = 0;
static int fps = 0;
int calc_fps() {

    frames++;
    if(pit_tick - last_tick >= 100)
    {
        fps = frames;
        frames = 0;
        last_tick = pit_tick;
    }
    return fps;
}

void smp_init_core() {
    // Enable SSE

    page_map_load(page_map_kernel);

    u64 cr0;
    u64 cr4;
    asm volatile("mov %%cr0, %0" :"=r"(cr0) :: "memory");
    asm volatile("mov %0, %%cr0" :: "r"((cr0 & ~(1 << 2)) | (1 << 1)) : "memory");
    asm volatile("mov %%cr4, %0" :"=r"(cr4) :: "memory");
    asm volatile("mov %0, %%cr4" :: "r"(cr4 | (3 << 9)) : "memory");

    lapic_init();

    printf("New CPU Online!\n");

    char buff[20];

    for (;;) {
        // CPU instructions here
        // FB as test :B
        fb_clear(vbe, 0xFF151515);
        wm_tick();
        wm_draw_cursor();
        sprintf(buff, "%d", calc_fps());
        fb_draw_string(vbe, font, buff, 0, 0, 0xFFFFFFFF);
        vbe_update();
    }
}

void smp_init_all() {
    struct limine_smp_response* smp_response = smp_request.response;
    printf("CPU count: %ld.\n", smp_response->cpu_count);
    if (smp_response->cpu_count > 1) {
        smp_response->cpus[1]->goto_address = smp_init_core;
    }
}