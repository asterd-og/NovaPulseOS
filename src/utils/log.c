#include <utils/log.h>
#include <libc/string.h>
#include <libc/printf.h>
#include <kernel/kernel.h>

char* pStatusMsg[] = {
    "GOOD",
    "INFO",
    "BAD "
};

uint32_t colors[] = {
    lightGreen,
    lightBlue,
    lightRed
};

void LogWrite(LogStatus status, char* pMsg, ...) {
    printf("[ ");
    
    FtSetFg(colors[status]);

    printf("%s", pStatusMsg[status]);

    FtResetFg();

    printf(" ] ");

    va_list args;
    va_start(args, pMsg);
    vfctprintf(putc, NULL, pMsg, args);
    va_end(args);
}