#pragma once

#include <types.h>

#define HEAP_MAGIC 0xA57B92

typedef struct {
    uint32_t magic;
    size_t pages;
} Chunk;

void  HpInit();
void* HpAlloc(size_t size);
void  HpFree(void* pPtr);