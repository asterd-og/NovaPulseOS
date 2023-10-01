#include <arch/x86_64/tables/gdt/gdt.h>

u64 table[] = {
    0x0000000000000000,
    
    0x00009a000000ffff, // 16 code
    0x000093000000ffff, // 16 data

    0x00cf9a000000ffff, // 32 code
    0x00cf93000000ffff, // 32 data

    0x00af9b000000ffff, // 64 code
    0x00af93000000ffff, // 64 data

    0x00affb000000ffff, // 64 usermode code
    0x00aff3000000ffff  // 64 usermode data
};

GDTR g_gdt;

void GdtInit() {
    g_gdt = (GDTR){
        .size = (sizeof(u64) * 9) - 1,
        .offset = (u64)&table
    };

    asm volatile ("lgdt %0" :: "m"(g_gdt) : "memory");
}