#include <utils/heap.h>
#include <arch/x86_64/mm/pfa.h>
#include <libc/string.h>
#include <kernel/kernel.h>

void* HpAlloc(size_t size) {
    if (size >= PmGetFreeMemory()) {
        SeFSend("Size is greater than free memory %ld x %ld\n", size, PmGetFreeMemory());
        return -1;
    }
    Chunk* pChk;
    
    u64* pPage = (u64*)(PmRequest((size >= 4096 ? alignUp(size, pageSize) : 1)) + hhdmOff);
    pChk = (Chunk*)pPage;
    
    pChk->magic = HEAP_MAGIC;
    pChk->pages = (size >= 4096 ? alignUp(size, pageSize) : 1);

    return (void*)pPage + sizeof(Chunk);
}

void HpFree(void* pPtr) {
    Chunk* chk = (Chunk*)(pPtr - sizeof(Chunk));
    PmFree((pPtr - sizeof(Chunk)) - hhdmOff, chk->pages);
    return;
}