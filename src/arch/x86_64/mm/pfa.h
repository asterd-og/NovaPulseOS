#pragma once

#include <types.h>

#define divRoundUp(x, y) (x + ( y - 1)) / y
#define alignUp(x, y) divRoundUp(x, y) * y

#define pageSize 4096

void  PmInit();
void* PmRequest(u8 size);
void  PmFree(void* pAddr, u8 num);