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

// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

void KeStart(void) {
    hhdmOff = hhdmReq.response->offset;
    pMmapRes = mmapReq.response;
    
    if (fbReq.response == NULL
     || fbReq.response->framebuffer_count < 1) {
        hcf();
    }

    GdtInit();

    PmInit();

    pFtCtx = flanterm_fb_simple_init(
        fbReq.response->framebuffers[0]->address,
        fbReq.response->framebuffers[0]->width,
        fbReq.response->framebuffers[0]->height,
        fbReq.response->framebuffers[0]->pitch
    );

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

    char* pStr = (char*)PmRequest(1) + hhdmOff;
    memcpy(pStr, "hey!", 5);
    printf("Allocated string: %s\n", pStr);
    PmFree(pStr, 1);
    //printf("Allocated string after free: %s\n", pStr);

    asm ("int $0x0");

    TermInit();

    while (1) {
        TermUpdate();
    }

    hcf();
}

void putchar_(char ch) {
    char str[] = {ch, '\0'};
    flanterm_write(pFtCtx, str, sizeof(str));
}

void putc(char ch, void* extra) {
    putchar_(ch);
}

void FtSetFg(uint32_t rgb) {
    pFtCtx->set_text_fg_rgb(pFtCtx, rgb);
}

void FtSetBg(uint32_t rgb) {
    pFtCtx->set_text_bg_rgb(pFtCtx, rgb);
}

void FtResetFg() {
    pFtCtx->set_text_fg_default(pFtCtx);
}

void FtResetBg() {
    pFtCtx->set_text_bg_default(pFtCtx);
}