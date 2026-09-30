#ifndef MEMORY_H
#define MEMORY_H

#include <types/types.h>

/**
 * Fills a block of memory with a constant byte value.
 *
 * Same semantics as the standard `memset`: writes `quantity` bytes
 * starting at `dest`, each set to `value`.
 *
 * @param dest Pointer to the block of memory to fill.
 * @param value Byte value to write into each byte of the block.
 * @param quantity Number of bytes to write.
 * @return dest, unchanged.
 */
void* mem_set(void* dest, u8 value, size_t quantity);

#endif