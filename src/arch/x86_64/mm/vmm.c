#include <arch/x86_64/mm/vmm.h>
#include <arch/x86_64/mm/pfa.h>
#include <libc/string.h>
#include <kernel/kernel.h>

#define flags 0b11

static u64* pPml4;

void VmmInit() {
    pPml4 = (u64*)PmRequest(1);
    memset((char*)pPml4 + hhdmOff, 0, pageSize);

    VmmMapRange(0, 4 * GiB);

    SeFSend("It worked %ld\n", sizeof(pPml4));
    // Here sizeof(pPml4) should be 8 because 2 * 4 = 8
    // we do this calculation because we got 4 bits of PML4
    // as we see in the SDM
    // (confirmed to be 8)

    asm volatile("mov %0, %%cr3" :: "a" (pPml4 + hhdmOff));

    // Past this line nuffin works

    SeFSend("It worked\n");
}

void VmmMapRange(u64 start, u64 end) {
    for (int i = alignDown(start, pageSize); i < alignDown(end, pageSize); i += pageSize) {
        VmmMapPage(i, i);
    }
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
    *(VmmGetPage(alignDown(virtAddr, pageSize))) = physAddr | flags;
    asm volatile("invlpg (%0)" :: "r" ((void*)pPml4));
}

u64* VmmGetPDPT(u64* pPt, u64 entry) {
    if ((pPt[entry] & 0b1) == 1) {
        // if the page exists
        return (u64*)(pPt[entry] & 0x000FFFFFFFFFF000);
        // we mask and return the page directory pointer table
        // which is basically the address
    } else {
        u64 page = (u64)PmRequest(1);
        memset((char*)page + hhdmOff, 0, pageSize);
        pPt[entry] = page | flags;
        return (u64*)(page & 0x000FFFFFFFFFF000);
    }
}