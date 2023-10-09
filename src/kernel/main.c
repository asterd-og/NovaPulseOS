#include <types.h>
#include <limine.h>
#include <arch/x86_64/tables/gdt/gdt.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/cpu/serial.h>
#include <arch/x86_64/mm/pfa.h>
#include <kernel/kernel.h>
#include <arch/x86_64/mm/vmm.h>

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

struct limine_memmap_response* pMmapRes;

// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

u64 bitExtracted(u64 number, int k, int p) {
    return (((1 << k) - 1) & (number >> (p - 1)));
}

void KeStart(void) {
    hhdmOff = hhdmReq.response->offset;
    pMmapRes = mmapReq.response;
    
    if (fbReq.response == NULL
     || fbReq.response->framebuffer_count < 1) {
        hcf();
    }

    GdtInit();
    IdtInit();
    SeInit();
    PmInit();
    VmmInit();

    struct limine_framebuffer *pFb = fbReq.response->framebuffers[0];

    for (size_t i = 0; i < 100; i++) {
        volatile u32 *fb_ptr = pFb->address;
        fb_ptr[i * (pFb->pitch / 4) + i] = 0xffffff;
    }

    asm volatile("int $0x0");

    hcf();
}

void putchar_(char ch) {}