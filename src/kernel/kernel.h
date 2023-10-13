#pragma once

#include <limine.h>
#include <types.h>

#define darkBlue     0x000080
#define darkYellow   0x808000
#define gray         0x808080
#define darkRed      0x800000
#define magenta      0xaa00aa
#define orange       0xff8000
#define lightRed     0xff8080
#define darkGreen    0x008000
#define cyan         0x00aaaa
#define lightGreen   0x80ff00
#define lightBlue    0x8080ff
#define lightYellow  0xffff80
#define white        0xffffff

extern u64 keEnd;

extern u64 hhdmOff;
extern struct flanterm_context *pFtCtx;

extern volatile struct limine_framebuffer_request fbReq;

extern volatile struct limine_hhdm_request hhdmReq;

extern volatile struct limine_memmap_request mmapReq;
extern struct limine_memmap_response* pMmapRes;

void putc(char ch, void* extra);