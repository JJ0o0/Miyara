#ifndef TSS_H
#define TSS_H

#include <types/types.h>

typedef struct {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 limit_flags;
    u8 base_high;
    u32 base_upper;
    u32 reserved;
} __attribute__((packed)) TSSDescriptor;

typedef struct {
    u32 reserved0;

    u64 rsp0;
    u64 rsp1;
    u64 rsp2;

    u64 reserved1;

    u64 ist1;
    u64 ist2;
    u64 ist3;
    u64 ist4;
    u64 ist5;
    u64 ist6;
    u64 ist7;

    u64 reserved2;
    u16 reserved3;
    u16 io_map_base;
} __attribute__((packed)) TSS;

_Static_assert(sizeof(TSSDescriptor) == 16, "TSS descriptor must be 16 bytes");
_Static_assert(sizeof(TSS) == 104, "TSS must be 104 bytes");

void tss_init(void);
void tss_set_rsp0(u64 rsp);

u64 tss_get_base(void);
u16 tss_get_limit(void);

#endif