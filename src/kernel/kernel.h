#pragma once

#include <limine.h>
#include <types.h>

extern u64 hhdmOff;

extern volatile struct limine_framebuffer_request fbReq;

extern volatile struct limine_hhdm_request hhdmReq;

extern volatile struct limine_memmap_request mmapReq;
extern struct limine_memmap_response* pMmapRes;