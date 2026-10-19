#include <cpu/tss.h>
#include <memory/memory.h>

static TSS tss;

extern u8 gdt_tss[];
extern void tss_load(void);

void tss_init(void) {
    mem_set(&tss, 0, sizeof(TSS));
    tss.io_map_base = sizeof(TSS);

    u64 base = (u64)&tss;
    u32 limit = sizeof(TSS) - 1;
    
    TSSDescriptor* descriptor = (TSSDescriptor*)gdt_tss;
    mem_set(descriptor, 0, sizeof(TSSDescriptor));

    descriptor->limit_low = limit & 0xFFFF;

    descriptor->base_low = base & 0xFFFF;
    descriptor->base_middle = (base >> 16) & 0xFF;
    descriptor->base_high = (base >> 24) & 0xFF;
    descriptor->base_upper = (base >> 32) & 0xFFFFFFFF;

    descriptor->access = 0x89;
    descriptor->limit_flags = (limit >> 16) & 0x0F;

    descriptor->reserved = 0;

    tss_load();
}

void tss_set_rsp0(u64 rsp) {
    tss.rsp0 = rsp;
}

u64 tss_get_base(void) {
    return (u64)&tss;
}

u16 tss_get_limit(void) {
    return sizeof(TSS) - 1;
}