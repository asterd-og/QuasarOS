#include <drivers/mouse.h>
#include <libc/lock.h>

u8 Mouse_State = 0;
u8 Mouse_Bytes[3];

u32 Mouse_X = 0;
u32 Mouse_Y = 0;

void Mouse_WaitWrite() {
    while ((inb(0x64) & 2) != 0) {;}
}

void Mouse_WaitRead() {
    while ((inb(0x64) & 1) != 1) {;}
}

void Mouse_Write(u8 value) {
    Mouse_WaitWrite();
    outb(0x64, 0xd4);
    Mouse_WaitWrite();
    outb(0x60, value);
}

u8 Mouse_Read() {
    Mouse_WaitRead();
    return inb(0x60);
}

static Locker Mouse_Lock;

void Mouse_Update(i8 accelX, i8 accelY) {
    Lock(&Mouse_Lock);
    
    Mouse_X += accelX;
    Mouse_Y -= accelY;

    if (Mouse_X < 0) Mouse_X = 0;
    if (Mouse_Y < 0) Mouse_Y = 0;
    if (Mouse_X > VBE->width) Mouse_X = VBE->width;
    if (Mouse_Y > VBE->height) Mouse_Y = VBE->height;
    
    Unlock(&Mouse_Lock);
}

void Mouse_Handler(Registers* regs) {
    (void)regs;
    u8 byte = inb(0x64);
    if ((!(byte & 1)) == 1) { Mouse_State = 0; return; }
    if ((!(byte & 2)) == 0) { Mouse_State = 0; return; }
    if (!(byte & 0x20)) { Mouse_State = 0; return; }
    switch (Mouse_State) {
        // Packet state
        case 0:
            Mouse_WaitRead();
            Mouse_Bytes[0] = Mouse_Read();
            Mouse_State++;
            break;
        case 1:
            Mouse_WaitRead();
            Mouse_Bytes[1] = Mouse_Read();
            Mouse_State++;
            break;
        case 2:
            Mouse_WaitRead();
            Mouse_Bytes[2] = Mouse_Read();
            
            if (Mouse_Bytes[0] & 0x80 || Mouse_Bytes[0] & 0x40) return;

            Mouse_Update(Mouse_Bytes[1], Mouse_Bytes[2]);

            Mouse_State = 0;
            break;
    }
}

void Mouse_Init() {
    u8 data;
    Mouse_WaitWrite();
    outb(0x64, 0xa8);
    Mouse_WaitWrite();
    outb(0x64, 0x20);

    Mouse_Read();
    data = (inb(0x60) | 2);

    Mouse_WaitWrite();
    outb(0x64, 0x60);
    Mouse_WaitWrite();
    outb(0x60, data);

    Mouse_Write(0xf6);
    Mouse_Read();
    Mouse_Write(0xf4);
    Mouse_Read();

    IRQ_Register(12, Mouse_Handler);
}