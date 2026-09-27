#ifndef PMM_H
#define PMM_H

#include <multiboot/multiboot.h>
#include <types/types.h>

typedef struct {
    u64 start;
    u64 length;
    u64 page_count;
    u8* bitmap;
} MemoryRegion;

void pmm_init(MBIHeader* mbi);
u64 pmm_alloc_page(void);
void pmm_free_page(u64 physical_address);

#endif