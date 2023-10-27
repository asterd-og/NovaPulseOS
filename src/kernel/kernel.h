#pragma once

#include <limine.h>
#include <types.h>

#define darkBlue     0xFF000080
#define darkYellow   0xFF808000
#define gray         0xFF808080
#define darkRed      0xFF800000
#define magenta      0xFFaa00aa
#define orange       0xFFff8000
#define lightRed     0xFFff8080
#define darkGreen    0xFF008000
#define cyan         0xFF00aaaa
#define lightGreen   0xFF80ff00
#define lightBlue    0xFF8080ff
#define lightYellow  0xFFffff80
#define white        0xFFffffff

extern u64 keEnd;

extern u64 hhdmOff;
extern struct flanterm_context *pFtCtx;

extern volatile struct limine_framebuffer_request fbReq;
extern struct limine_framebuffer_response* pFbRes;

extern volatile struct limine_hhdm_request hhdmReq;

extern volatile struct limine_memmap_request mmapReq;
extern struct limine_memmap_response* pMmapRes;

void putc(char ch, void* extra);