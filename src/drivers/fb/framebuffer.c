#include <drivers/fb/framebuffer.h>
#include <kernel/kernel.h>
#include <utils/heap.h>

u64 fbWidth;
u64 fbHeight;
u64 fbPitch;
u16 fbBpp;

u32* pFbAddr;
u32* pBackAddr;

psf2Hdr* pFnt;
u8* pFntStart;
u16 fntPitch;

int cx = 0;
int cy = 0;

struct limine_framebuffer* pFb;

int FbInit(u64* pPsfAddr) {
    pFnt = (psf2Hdr*)pPsfAddr;
    pFb  = pFbRes->framebuffers[0];

    if (pFnt->magic[0] != 0x72 || pFnt->magic[1] != 0xb5 || pFnt->magic[2] != 0x4a || pFnt->magic[3] != 0x86) {
        return 1;
    }

    fntPitch = pFnt->charSize / pFnt->height;
    pFntStart = (u8*)pFnt + pFnt->headerSize;

    fbWidth = pFb->width;
    fbHeight = pFb->height;
    fbPitch = pFb->pitch;

    for (size_t i = 0; i < pMmapRes->entry_count; i++) {
        if (pMmapRes->entries[i]->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE || pMmapRes->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            if (pMmapRes->entries[i]->length >= fbWidth * fbHeight * 4) {
                pBackAddr = (u32*)(pMmapRes->entries[i]->base + hhdmOff);
                pMmapRes->entries[i]->base += fbWidth * fbHeight * 4;
                pMmapRes->entries[i]->length -= fbWidth * fbHeight * 4;
                break;
            }
        }
    }

    pFbAddr = pFb->address;

    memset(pBackAddr, 0, fbWidth * fbHeight * 4);

    return 0;
}

void FbSetPix(u32 x, u32 y, u32 color) {
    pBackAddr[y * fbWidth + x] = color;
}

void FbDrawFillRect(u32 x, u32 y, u32 width, u32 height, u32 color) {
    for (int yy = y; yy < y + height; yy++) {
        for (int xx = x; xx < x + width; xx++) {
            FbSetPix(xx, yy, color);
        }
    }
}

void FbWriteChar(char c, u32 fg, u32 bg) {
    if (c == '\n' || c == '\r') {
        cx = 0;
        cy += pFnt->height;
        return;
    }

    if (c == '\b') {
        cx -= pFnt->width;
        return;
    }

    u8* ch = pFntStart + c * pFnt->charSize;
    FbDrawFillRect(cx, cy, pFnt->width, pFnt->height, bg);

    for (size_t yy = 0; yy < pFnt->height; yy++) {
        for (size_t xx = 0; xx < pFnt->width; xx++) {
            if ((ch[yy * fntPitch + xx / 8] >> (7 - xx % 8)) & 1) {
                FbSetPix(cx + xx, cy + yy, fg);
            }
        }
    }

    cx += pFnt->width;
}

void FbClear(u32 color) {
    for (int i = 0; i < fbWidth * fbHeight; i++) pBackAddr[i] = color;
}

void FbUpdate() {
    for (int i = 0; i < fbWidth * fbHeight; i++) pFbAddr[i] = pBackAddr[i];
}

u64 FbGetCX() {
    return cx;
}

u64 FbGetCY() {
    return cy;
}

u64* FbGetFnt() {
    return pFnt;
}