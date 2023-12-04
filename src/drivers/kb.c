#include <drivers/kb.h>
#include <libc/lock.h>
#include <arch/x86_64/cpu/serial.h>

bool KB_KeyPressed = false;
char KB_CurrentChar = '\0';
bool KB_Caps = false;
bool KB_Shift = false;

Locker KB_Lock;

void KB_Handler(void) {
    Lock(&KB_Lock);
    u8 key = inb(0x60);

    if (!(key & 0x80)) {
        // Key was pressed
        switch (key) {
            case 0x2a:
                // Shift
                KB_Shift = true;
                break;
            case 0x3a:
                // Caps
                KB_Caps = !KB_Caps;
                break;
            default:
                // Letter(?)
                KB_KeyPressed = true;
                if (KB_Shift) KB_CurrentChar = KB_MapKeysShift[key];
                else if (KB_Caps) KB_CurrentChar = KB_MapKeysCaps[key];
                else KB_CurrentChar = KB_MapKeys[key];
                break;
        }
    } else {
        switch (key) {
            case 0xaa:
                // Shift was released
                KB_Shift = false;
                break;
        }
    }
    Unlock(&KB_Lock);
}


char KB_GetChar() {
    Lock(&KB_Lock);
    if (KB_KeyPressed) {
        KB_KeyPressed = false;
        Unlock(&KB_Lock);
        return KB_CurrentChar;
    } else {
        Unlock(&KB_Lock);
        return '\0';
    }
}

void KB_Init() {
    IRQ_Register(1, KB_Handler);
}