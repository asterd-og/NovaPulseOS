#include <arch/x86_64/mm/vmm.h>
#include <arch/x86_64/mm/pfa.h>
#include <libc/string.h>
#include <kernel/kernel.h>
#include <limine.h>

#define Present 1
#define Writable 2

typedef u64 pdp;
typedef u64 pd;
typedef u64 pt;
typedef u64 page;

pdp pml4[512];

volatile struct limine_kernel_address_request keAddrReq = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

u64 kePhysAddr;
u64 keVirtAddr;

void VmmInit() {
    u64* pPml4 = &pml4;
    asm volatile("mov %%cr3, %0" : "=r"(pPml4) :: "memory");

    asm volatile("mov %0, %%cr3" :: "a" ((void*)pPml4));

    SeFSend("It worked\n");
}