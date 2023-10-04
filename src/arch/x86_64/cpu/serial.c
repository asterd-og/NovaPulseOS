#include <arch/x86_64/cpu/serial.h>
#include <arch/x86_64/io.h>
#include <libc/string.h>
#include <libc/printf.h>

int SeInit() {
    outb(COM1 + 1, 0);
    outb(COM1 + 3, 0x80);
    outb(COM1, 0x03);
    outb(COM1 + 1, 0);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    outb(COM1 + 4, 0x1E);
    outb(COM1, 0xAE);

    if (inb(COM1) != 0xAE) {
        return 1;
    }

    outb(COM1 + 4, 0x0F);
    return 0;
}

int SeIsBusEmpty() {
    return inb(COM1 + 5) & 0x20;
}

void SeWriteChar(char a) {
    while (SeIsBusEmpty() == 0);
    outb(COM1, a);
}

void SeSend(char* pStr) {
    while (*pStr != '\0') {
        SeWriteChar(*pStr++);
    }
}

void SeWrap(char c, void* extra) {
    SeWriteChar(c);
}

void SeFSend(char* pStr, ...) {
    va_list args;
    va_start(args, pStr);
    const int ret = vfctprintf(SeWrap, NULL, pStr, args);
    va_end(args);
}