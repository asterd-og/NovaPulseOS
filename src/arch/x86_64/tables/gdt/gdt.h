#pragma once

#include <types.h>

typedef struct {
    u16 size;
    u64 offset;
} __attribute__((packed))
GDTR;

void GdtInit();