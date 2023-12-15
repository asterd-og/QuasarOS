#include <drivers/kb.h>
#include <libc/lock.h>
#include <arch/x86_64/cpu/serial.h>
#include <sched/sched.h>

bool kb_key_pressed = false;
char kb_current_char = '\0';
bool kb_caps = false;
bool kb_shift = false;

void kb_handler(registers* regs) {
    sched_lock();
    (void)regs;

    u8 key = inb(0x60);

    if (!(key & 0x80)) {
        // Key was pressed
        switch (key) {
            case 0x2a:
                // Shift
                kb_shift = true;
                break;
            case 0x3a:
                // Caps
                kb_caps = !kb_caps;
                break;
            default:
                // Letter(?)
                kb_key_pressed = true;
                if (kb_shift) kb_current_char = kb_map_keys_shift[key];
                else if (kb_caps) kb_current_char = kb_map_keys_caps[key];
                else kb_current_char = kb_map_keys[key];
                break;
        }
    } else {
        switch (key) {
            case 0xaa:
                // Shift was released
                kb_shift = false;
                break;
        }
    }

    sched_unlock();
}


char kb_get_char() {
    if (kb_key_pressed) {
        kb_key_pressed = false;
        return kb_current_char;
    } else {
        return '\0';
    }
}

void kb_init() {
    irq_register(1, kb_handler);
}