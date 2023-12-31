#include <arch/x86_64/mm/pfa.h>
#include <arch/x86_64/cpu/serial.h>
#include <kernel/kernel.h>
#include <libc/string.h>

#define memUsable 0
#define memBLReclaimable 5

u8* pBitmap;
size_t bitmapSize;

size_t lastIdx = 0;
size_t freePages = 0;

void bitSet(size_t idx) {
    pBitmap[idx / 8] |= (1 << (idx % 8));
}

void bitClear(size_t idx) {
    pBitmap[idx / 8] &= ~(1 << (idx % 8));
}

size_t  bitGet(size_t idx) {
    return (pBitmap[idx / 8] & (1 << (idx % 8)));
}

u64 freeMemory = 0;

char* pMemType[] = {
    "LIMINE_MEMMAP_USABLE",
    "LIMINE_MEMMAP_RESERVED",
    "LIMINE_MEMMAP_ACPI_RECLAIMABLE",
    "LIMINE_MEMMAP_ACPI_NVS",
    "LIMINE_MEMMAP_BAD_MEMORY",
    "LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE",
    "LIMINE_MEMMAP_KERNEL_AND_MODULES",
    "LIMINE_MEMMAP_FRAMEBUFFER",
};

void PmInit() {
    u64 topAddr;
    u64 higherAddr;
    
    for (size_t i = 0; i < pMmapRes->entry_count; i++) {
        SeFSend("Memory @ 0x%lx = Type: %s Length: %lx\n", pMmapRes->entries[i]->base, pMemType[pMmapRes->entries[i]->type], pMmapRes->entries[i]->length);
        if (pMmapRes->entries[i]->type == memUsable) {
            topAddr = pMmapRes->entries[i]->base + pMmapRes->entries[i]->length;
            if (topAddr > higherAddr) {
                higherAddr = topAddr;
            }
        }
    }

    freeMemory = higherAddr;
    bitmapSize = alignUp((higherAddr / pageSize) / 8, pageSize);

    for (size_t j = 0; j < pMmapRes->entry_count; j++) {
        if (pMmapRes->entries[j]->type == memUsable) {
            if (pMmapRes->entries[j]->length >= bitmapSize) {
                SeFSend("Found bitmap sized entry at 0x%lx virt: 0x%lx\n", pMmapRes->entries[j]->base, pMmapRes->entries[j]->base + hhdmOff);
                pBitmap = (u8*)(pMmapRes->entries[j]->base + hhdmOff);
                pMmapRes->entries[j]->base += bitmapSize;
                pMmapRes->entries[j]->length -= bitmapSize;
                break;
            }
        }
    }

    memset(pBitmap, 0xff, bitmapSize);

    for (size_t k = 0; k < pMmapRes->entry_count; k++) {
        if (pMmapRes->entries[k]->type == memUsable) {
            for (size_t l = 0; l < pMmapRes->entries[k]->length; l += pageSize) {
                freePages++;
                bitClear((pMmapRes->entries[k]->base + l) / pageSize);
            }
        }
    }
}

u64 PmFindFree(u64 num) {
    for (size_t i = 0; i < num;) {
        if (lastIdx > bitmapSize) {
            if (freePages < num) {
                return -1;
            }   
            lastIdx = 0;
            return PmFindFree(num);
        } else if (bitGet(i + lastIdx) == 0) {
            i++;
        } else {
            i = 0;
            lastIdx++;
        }
    }
    return lastIdx;
}

void* PmRequest(u64 num) {
    u64 pageIdx = PmFindFree(num);

    SeFSend("Found usable ones.\n");

    if (pageIdx == (u64)-1) {
        SeFSend("PFA: No free %ld pages only %ld\n", num, freePages);
        return -1;
    }

    for (size_t i = pageIdx; i < pageIdx + num; i++) {
        bitSet(i);
    }

    return (void*)(pageIdx * pageSize);
}

void PmFree(void* pPtr, u64 num) {
    uptr pageIdx = (u64)pPtr;
    pageIdx = (pageIdx) / pageSize;
    for (u64 i = pageIdx; i < pageIdx + num; i++) {
        bitClear(i);
    }
}

size_t PmGetFreePages() {
    return freePages;
}

u64 PmGetFreeMemory() {
    return freeMemory;
}