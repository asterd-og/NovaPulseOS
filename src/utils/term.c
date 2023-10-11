#pragma once

#include <utils/term.h>
#include <libc/string.h>
#include <libc/printf.h>
#include <kernel/kernel.h>

char buffer[512];
int bufferIdx = 0;
char c = '\0';

void TermInput() {
    FtResetFg();
    printf("> ");
    FtSetFg(white);
}

void TermInit() {
    printf("\nTerminal Initialised.\n\n");

    printf("   |\\---/|\n");
    printf("   | ,_, |\n");
    printf("    \\_`_/-..----.\n");
    printf(" ___/ `   ' ,""+ \\  sk\n");
    printf("(__...'   __\\    |`.___.';\n");
    printf("  (_,...'(_,.`__)/'.....+\n\n");

    TermInput();
}

void TermUpdate() {
    c = KbGetChar();
    if (c != '\0') {
        if (c == '\n') {
            printf("\n%s\n", buffer);
            memset(buffer, 0, 512);
            bufferIdx = 0;
            TermInput();
        } else if (c == '\b') {
            if (bufferIdx > 0) {
                printf("\b \b");
                bufferIdx--;
                buffer[bufferIdx] = '\0';
            }
        } else {
            buffer[bufferIdx] = c;
            bufferIdx++;
            printf("%c", c);
        }
    }
}