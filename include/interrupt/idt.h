#ifndef IDT_H
#define IDT_H

#include <types/types.h>

typedef struct {
    u16 offset_low;
    u16 selector;
    u8  ist;
    u8  type_attr;
    u16 offset_mid;
    u32 offset_high;
    u32 reserved;
} IDTEntry;

typedef struct __attribute__((packed)) {
    u16 limit;
    u64 base;
} IDTR;

typedef IDTEntry IDTTable[256];

_Static_assert(sizeof(IDTEntry) == 16, "IDTEntry must be 16 bytes.");
_Static_assert(sizeof(IDTR) == 10, "IDTR must be 10 bytes.");

#endif