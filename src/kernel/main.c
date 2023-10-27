#include <types.h>
#include <limine.h>
#include <arch/x86_64/tables/gdt/gdt.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/cpu/serial.h>
#include <arch/x86_64/mm/pfa.h>
#include <kernel/kernel.h>
#include <arch/x86_64/mm/vmm.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <utils/log.h>
#include <drivers/ps2/kb.h>
#include <utils/term.h>
#include <utils/heap.h>
#include <drivers/fb/framebuffer.h>

u64 hhdmOff;

volatile struct limine_framebuffer_request fbReq = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

volatile struct limine_hhdm_request hhdmReq = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

volatile struct limine_memmap_request mmapReq = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

struct flanterm_context *pFtCtx;

struct limine_memmap_response* pMmapRes;
struct limine_framebuffer_response* pFbRes;

// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

static volatile struct limine_module_request modReq = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

struct limine_file* findModule(int pos) {
    return modReq.response->modules[pos];
}

u32 color = 0xFFFFFFFF;

void KeStart(void) {
    hhdmOff = hhdmReq.response->offset;
    pMmapRes = mmapReq.response;
    pFbRes = fbReq.response;
    
    if (fbReq.response == NULL
     || fbReq.response->framebuffer_count < 1) {
        hcf();
    }

    GdtInit();

    if (FbInit(findModule(0)->address)) {
        SeFSend("Could not load PSF2 font.\n");
    }

    PmInit();

    LogWrite(Good, "GDT Initialised.\n");
    LogWrite(Good, "PMM Initialised.\n");

    asm volatile("cli");

    IdtInit();
    LogWrite(Good, "IDT Initialised.\n");

    PicRemap();
    LogWrite(Good, "PIC Remmaped.\n");

    asm volatile("sti");

    SeInit();
    LogWrite(Good, "Serial Initialised.\n");

    VmmInit();
    LogWrite(Good, "VMM Initialised.\n");

    KbInit();
    LogWrite(Good, "Keyboard Initialised.\n");


    /*TermInit();

    while (1) {
        TermUpdate();
    }*/

    printf("Hello World!\n");

    FbUpdate();

    hcf();
}

void putchar_(char ch) {
    FbWriteChar(ch, color);
    //flanterm_write(pFtCtx, str, sizeof(str));
}

void putc(char ch, void* extra) {
    putchar_(ch);
}

void FtSetFg(uint32_t rgb) {
    color = rgb;
    //pFtCtx->set_text_fg_rgb(pFtCtx, rgb);
}

void FtSetBg(uint32_t rgb) {
    //pFtCtx->set_text_bg_rgb(pFtCtx, rgb);
}

void FtResetFg() {
    color = 0xFFFFFFFF;
    //pFtCtx->set_text_fg_default(pFtCtx);
}

void FtResetBg() {
    //pFtCtx->set_text_bg_default(pFtCtx);
}