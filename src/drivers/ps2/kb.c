#include <drivers/ps2/kb.h>
#include <drivers/ps2/kbMap.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/io.h>

bool pressed = false;
char currentCh = '\0';
bool caps = false;
bool shift = false;

void KbHandler(Registers* regs) {
    u8 key = inb(0x60);
    if (!(key & 0x80)) {
        if (key == 0x2a) {
            SeFSend("shift pressed\n");
            shift = true;
        } else if (key == 0x3a) {
            SeFSend("caps pressed\n");
            caps = !caps;
        } else {
            pressed = true;
            if (shift) currentCh = keysShift[key];
            else if (caps) currentCh = keysCaps[key];
            else currentCh = keys[key];
        }
    } else {
        if (key == 0xaa) {
            shift = false;
        }
    }
}

void KbInit() {
    IrqRegister(1, KbHandler);
}

char KbGetChar() {
    if (pressed) {
        pressed = false;
        return currentCh;
    } else {
        return '\0';
    }
}