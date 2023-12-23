#include <libc/panic.h>
#include <arch/x86_64/tables/idt/idt.h>

void panic(const char* fmt, ...) {
    irq_unregister(0); // Stop tasking
    sched_lock();
    wm_panic_destroy();
    
    __asm__ ("cli");
    
    va_list args;
    va_start(args, fmt);
    
    char buffer[1024];
    vsprintf(buffer, fmt, args);
    fb_clear(vbe, 0xFFEE0000);
    vbe_update();
    
    va_end(args);

    for (;;) {
        __asm__ ("hlt");
    }
}