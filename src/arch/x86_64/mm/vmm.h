#pragma once

#include <types.h>

void VmmInit();
u64* VmmGetPage(u64 virtAddr);
void VmmMapPage(u64 virtAddr, u64 physAddr);
u64* VmmGetPDPT(u64* pPt, u64 entry);