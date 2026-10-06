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

/**
 * Copies a block of memory from one location to another.
 *
 * Same semantics as the standard `memcpy`: copies `quantity` bytes
 * from src to dest. The memory regions must not overlap.
 *
 * @param dest Destination memory block.
 * @param src Source memory block.
 * @param quantity Number of bytes to copy.
 * @return dest, unchanged.
 */
void* mem_copy(void* dest, const void* src, size_t quantity);

#endif