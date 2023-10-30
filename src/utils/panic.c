#include <utils/panic.h>
#include <libc/string.h>
#include <libc/printf.h>
#include <kernel/kernel.h>
#include <drivers/fb/framebuffer.h>

void Panic(char* pStr, ...) {
    FtSetFg(0);
    FtSetBg(lightRed);

    printf(":C PANIC: ");

    va_list args;
    va_start(args, pStr);
    vfctprintf(putc, NULL, pStr, args);
    va_end(args);

    FbUpdate();

    FtResetFg();
    FtResetBg();
}