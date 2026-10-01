#ifndef HEAP_H
#define HEAP_H

#include <types/types.h>

#define HEAP_START 0xFFFF900000000000ULL
#define HEAP_SIZE (16ULL * 1024 * 1024)
#define HEAP_END (HEAP_START + HEAP_SIZE)

void heap_init(void);
void* k_malloc(u64 size);
void k_free(void* ptr);

#endif