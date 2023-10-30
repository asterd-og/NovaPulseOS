#include <utils/term.h>
#include <utils/log.h>
#include <libc/string.h>
#include <libc/printf.h>
#include <kernel/kernel.h>
#include <arch/x86_64/mm/pfa.h>
#include <arch/x86_64/cpu/serial.h>
#include <drivers/ps2/kb.h>
#include <drivers/fb/framebuffer.h>

char buffer[512];
int bufferIdx = 0;
char c = '\0';

u16 fntWidth;
u16 fntHeight;
psf2Hdr* _pFnt;

typedef void(*funcHandlerPtr)(void);
typedef struct {
    char const *name;
    funcHandlerPtr exec;
    char const *help;
} CmdStruct;

void CmdFetch(void);

const CmdStruct cmds[] = {
    {"fetch", &CmdFetch, "Fetch system info"},
    {"",0,""}
};

void TermCmdHandler(char* cmd)
{
    int i = 0;
    while(cmds[i].exec > 0)
    {
        if(!strcmp(cmds[i].name, cmd))
        {
            (*cmds[i].exec)();
            return;
        }
        i++;
    }

    FtSetFg(lightRed);
    printf("Couldn't find command '%s'.\n", cmd);
    FtResetFg();
}

void TermInput() {
    FtResetFg();
    printf("> ");
    FtSetFg(white);
}

void TermCurErase() {
    FbDrawFillRect(FbGetCX(), FbGetCY() + (fntHeight - 4), fntWidth, 4, 0x0);
}

void TermCurDraw() {
    FbDrawFillRect(FbGetCX(), FbGetCY() + (fntHeight - 4), fntWidth, 4, white);
}

void TermInit() {
    _pFnt = (psf2Hdr*)FbGetFnt();
    fntWidth = _pFnt->width;
    fntHeight = _pFnt->height;
    
    SeFSend("%d %d\n", fntWidth, fntHeight);

    printf("\nTerminal Initialised.\n\n");

    printf("   |\\---/|\n");
    printf("   | ,_, |\n");
    printf("    \\_`_/-..----.\n");
    printf(" ___/ `   ' ,""+ \\  sk\n");
    printf("(__...'   __\\    |`.___.';\n");
    printf("  (_,...'(_,.`__)/'.....+\n\n");

    TermInput();
    TermCurDraw();
}

void TermUpdate() {
    c = KbGetChar();
    if (c != '\0') {
        TermCurErase();
        if (c == '\n') {
            printf("\n");
            TermCmdHandler(buffer);
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
        TermCurDraw();
    }
}

void CmdFetch(void) {
    printf("\n |\\__/,|   (`\\  ");
    FtSetFg(cyan);
    printf("NovaPulseOS:\n");
    FtSetFg(white);
    printf(" |_ _  |.--.) )  - Free Physical Pages: ");
    FtSetFg(lightGreen);
    printf("%ld\n", PmGetFreePages());
    FtSetFg(white);
    printf(" ( T   )     /\n");
    printf("(((^_(((/(((_/\n\n");
}