#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <types/types.h>

typedef struct {
    u32 type;
    u32 size;
} TagHeader;

typedef struct {
    u32 total_size;
    u32 reserved;
} MBIHeader;

typedef struct {
    u32 type;
    u32 size;
    u32 entry_size;
    u32 entry_version;
} MemoryMapTag;

typedef struct {
    u64 addr;
    u64 len;
    u32 type;
    u32 reserved;
} MemoryMapEntry;

#endif