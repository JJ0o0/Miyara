#ifndef PAGING_H
#define PAGING_H

#include <types/types.h>

#define PAGE_PRESENT  0x1
#define PAGE_WRITABLE 0x2
#define PAGE_USER     0x4
typedef u64 PageTableEntry;
typedef PageTableEntry PageTable[512];

void paging_init(void);

#endif