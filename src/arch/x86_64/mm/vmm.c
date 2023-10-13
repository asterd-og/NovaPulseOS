#include <arch/x86_64/mm/vmm.h>
#include <arch/x86_64/mm/pfa.h>
#include <libc/string.h>
#include <kernel/kernel.h>
#include <limine.h>

#define flags 0b11
static u64* pPml4;

volatile struct limine_kernel_address_request keAddrReq = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

u64 kePhysAddr;
u64 keVirtAddr;

void VmmInit() {
    asm volatile("mov %%cr3, %0" : "=r"(pPml4) :: "memory");
    /*pPml4 = (u64*)PmRequest(1);
    memset((char*)pPml4 + hhdmOff, 0, pageSize);

    kePhysAddr = keAddrReq.response->physical_base;
    keVirtAddr = keAddrReq.response->virtual_base;
    u64 keSize = alignUp(keEnd, pageSize) - keVirtAddr;

    VmmMapRange(0, keSize, keVirtAddr, kePhysAddr);
    
    VmmMapRange(pageSize, 0x100000000, 0, 0);
    VmmMapRange(pageSize, 0x100000000, hhdmOff, 0);

    u64 base;
    u64 top;

    for (u64 i = 0; i < pMmapRes->entry_count; i++) {
        base = alignDown(pMmapRes->entries[i]->base, pageSize);
        top  = alignUp(pMmapRes->entries[i]->base + pMmapRes->entries[i]->length, pageSize);
        if (top <= 0x100000000) {
            continue;
        }
        for (u64 j = base; j < top; j += pageSize) {
            if (j < 0x100000000) {
                continue;
            }
            VmmMapPage(j, j);
            VmmMapPage(j + hhdmOff, j);
        }
    }*/

    asm volatile("mov %0, %%cr3" :: "a" ((void*)pPml4));

    SeFSend("It worked\n");
}

void VmmMapRange(u64 start, u64 end, u64 virtOff, u64 physOff) {
    for (u64 i = alignDown(start, pageSize); i < alignUp(end, pageSize); i += pageSize) {
        VmmMapPage(i + virtOff, i + physOff);
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

void VmmSetPage(u64 virtAddr, u64 physAddr) {
    u64 tableIdx = (virtAddr & ((u64)0b111111111 << 12)) >> 12;
    u64 de = (virtAddr & ((u64)0b111111111 << 21)) >> 21;
    u64 dpe = (virtAddr & ((u64)0b111111111 << 30)) >> 30;
    u64 pml4 = (virtAddr & ((u64)0b111111111 << 39)) >> 39;

    u64* directoryEntry = VmmGetPDPT(pPml4, pml4);
    u64* directoryPointerEntry = VmmGetPDPT(directoryEntry, dpe);
    u64* table = VmmGetPDPT(directoryPointerEntry, dpe);

    table[tableIdx] = physAddr | flags;

    asm volatile("invlpg (%0)" :: "r" ((void*)pPml4));
}

void VmmMapPage(u64 virtAddr, u64 physAddr) {
    VmmSetPage((alignDown(virtAddr, pageSize)), physAddr);
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