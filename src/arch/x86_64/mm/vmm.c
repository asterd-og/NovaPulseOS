#include <arch/x86_64/mm/vmm.h>
#include <arch/x86_64/mm/pfa.h>
#include <libc/string.h>
#include <kernel/kernel.h>

#define flags 0b11

u64* pPml4;

// TODO: FIX PAGE FAULT AT MEMSETS BECAUSE PFA'S RETURNED PAGE
// SHOULD BE OFFSETTED BY HHDM, BUT OFFSETTING IT CAUSES GPF

void VmmInit() {
    pPml4 = (u64*)PmRequest(1);
    for (int i = 0; i < 4096; i++) pPml4[i] = 0;
    //memset(pPml4, 0, pageSize);

    for (int i = pageSize; i < 4ULL * 1024ULL * 1024ULL * 1024ULL; i += pageSize) {
        // Loop through the first 4 GiB and map those pages
        SeFSend("Mapping page %x\n", i);
        VmmMapPage(i, i);
    }

    asm volatile("mov %0, %%cr3": :"a"(pPml4));
}

u64* VmmGetPage(u64 virtAddr) {
    u64 tableIdx = (virtAddr & ((u64)0b111111111 << 12)) >> 12;
    u64 de = (virtAddr & ((u64)0b111111111 << 21)) >> 21;
    u64 dpe = (virtAddr & ((u64)0b111111111 << 30)) >> 30;
    u64 pml4 = (virtAddr & ((u64)0b111111111 << 39)) >> 39;

    u64* directoryEntry = VmmGetPDPT(pPml4, pml4);
    u64* directoryPointerEntry = VmmGetPDPT(directoryEntry, dpe);
    u64* table = VmmGetPDPT(directoryPointerEntry, dpe);
    
    // we do all this because virtual memory is a hierarchy

    return &table[tableIdx];
}

void VmmMapPage(u64 virtAddr, u64 physAddr) {
    *(VmmGetPage(virtAddr)) = physAddr | flags;
    asm volatile("invlpg (%0)" ::"r" ((u64)pPml4) : "memory");
}

u64* VmmGetPDPT(u64* pPt, u64 entry) {
    if ((pPt[entry] & 0b1) == 1) {
        // if the page exists
        return (u64*)(pPt[entry] & 0x000FFFFFFFFFF000);
        // we mask and return the page directory pointer table
    } else {
        u64 page = (u64)PmRequest(1);
        memset(page, 0, pageSize);
        pPt[entry] = page | flags;
        return (u64*)(pPt[entry] & 0x000FFFFFFFFFF000);
    }
}