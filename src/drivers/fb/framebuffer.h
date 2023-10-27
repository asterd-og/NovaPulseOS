#pragma once

#include <types.h>

#define PSF2_MAGIC0 0x72
#define PSF2_MAGIC1 0xb5
#define PSF2_MAGIC2 0x4a
#define PSF2_MAGIC3 0x86

typedef struct {
    u8  magic[4];
    u32 version;
    u32 headerSize;
    u32 flags;
    u32 length;
    u32 charSize;
    u32 height, width;
} __attribute__((packed)) psf2Hdr;

int FbInit();

void FbSetPix(u32 x, u32 y, u32 color);

void FbDrawFillRect(u32 x, u32 y, u32 width, u32 height, u32 color);

void FbWriteChar(char c, u32 color, u32 bg);

void FbClear(u32 color);
void FbUpdate();

u64 FbGetCX();
u64 FbGetCY();
u64* FbGetFnt();