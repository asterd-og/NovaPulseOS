#pragma once

#include <types.h>

#define divRoundUp(x, y) (x + ( y - 1)) / y
#define alignUp(x, y) divRoundUp(x, y) * y
#define alignDown(x, y)  (x / y) * y

#define GiB 0x40000000UL

#define pageSize 4096

void   PmInit();
void*  PmRequest(u64 size);
void   PmFree(void* pAddr, u64 num);
size_t PmGetFreePages();
u64    PmGetFreeMemory();