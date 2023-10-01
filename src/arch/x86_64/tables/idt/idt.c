#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/cpu/serial.h>
#include <arch/x86_64/cpu/pic.h>

__attribute__((aligned(0x10)))
static IDTEntry s_idtEntries[256];
static IDTR     s_idtr;
extern void*    g_pIsrTable[];

HandlerFunc     g_handlers[16];

static const char* s_pMsg[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

void IdtSetDesc(u8 vec, void* pIsr) {
    IDTEntry* pDesc = &s_idtEntries[vec];

    pDesc->low  = (u64)pIsr & 0xFFFF;
    pDesc->cs   = 0x28;
    pDesc->ist  = 0;
    pDesc->attr = 0x8E;
    pDesc->mid  = ((u64)pIsr >> 16) & 0xFFFF;
    pDesc->high = ((u64)pIsr >> 32) & 0xFFFFFFFF;
    pDesc->resv = 0;
}

void IdtInit() {
    asm volatile("cli");

    s_idtr = (IDTR){
        .size   = (u16)sizeof(IDTEntry) * 256 - 1,
        .offset = (uintptr_t)&s_idtEntries[0]
    };

    for (u8 vec = 0; vec < 32; vec++) {
        IdtSetDesc(vec, g_pIsrTable[vec]);
    }

    PicRemap();

    asm volatile ("lidt %0" :: "m"(s_idtr));
    asm volatile("sti");
}

void IrqRegister(u8 vec, HandlerFunc pHandler) {
    g_handlers[vec] = pHandler;
}

void IntHandler(Registers* pRegs) {
    if (pRegs->intNo < 32) {
        asm volatile("cli");

        SeSend("Uh oh! something went wrong.\n");
        SeSend(s_pMsg[pRegs->intNo]);
        SeSend("\n");

        for (;;)asm volatile("hlt");
    } else {
        g_handlers[pRegs->intNo - 32](pRegs);
        PicEoi(pRegs->intNo - 32);
    }
}