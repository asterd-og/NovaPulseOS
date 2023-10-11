#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/cpu/serial.h>
#include <arch/x86_64/cpu/pic.h>

__attribute__((aligned(0x10)))
static IDTEntry s_idtEntries[256];
static IDTR     s_idtr;
extern void*    g_pIntTable[];

void*  g_handlers[16] = {0};

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
  //  IDTEntry* pDesc = &s_idtEntries[vec];

    s_idtEntries[vec].low  = (u64)pIsr & 0xFFFF;
    s_idtEntries[vec].cs   = 0x28;
    s_idtEntries[vec].ist  = 0;
    s_idtEntries[vec].attr = (u8)0x8E;
    s_idtEntries[vec].mid  = ((u64)pIsr >> 16) & 0xFFFF;
    s_idtEntries[vec].high = ((u64)pIsr >> 32) & 0xFFFFFFFF;
    s_idtEntries[vec].resv = 0;
}

void IdtInit() {
    for (u8 vec = 0; vec < 48; vec++) {
        IdtSetDesc(vec, g_pIntTable[vec]);
    }

    s_idtr = (IDTR){
        .size   = (u16)sizeof(IDTEntry) * 256 - 1,
        .offset = (u64)s_idtEntries
    };

    asm ("lidt %0" :: "m"(s_idtr));
}

void IrqRegister(u8 vec, void* pHandler) {
    g_handlers[vec] = pHandler;
}

void IsrHandler(Registers* pRegs) {
    asm volatile("cli");

    SeSend("Uh oh! something went wrong.\n");
    SeSend(s_pMsg[pRegs->intNo]);
    SeSend("\n");

    for (;;)asm volatile("hlt");
}

void IrqHandler(Registers* pRegs) {
    void(*HandlerFunc)(Registers*) = g_handlers[pRegs->intNo - 32];

    if ((u64)g_handlers[pRegs->intNo - 32] != 0) {
        HandlerFunc(pRegs);
    }
    
    PicEoi(pRegs->intNo - 32);
}