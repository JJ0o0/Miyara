#ifndef HEAP_H
#define HEAP_H

#include <types/types.h>

/**
 * Virtual base address of the kernel heap.
 *
 * Placed in the higher half, after the direct physical memory map
 * (see DIRECT_MAP_BASE in paging.h).
 */
#define HEAP_START 0xFFFF900000000000ULL

/**
 * Maximum size of the kernel heap, in bytes (16MB).
 *
 * This is the size of the reserved virtual range, not of the memory
 * actually in use: pages are only mapped (and backed by physical
 * memory) as the heap grows.
 */
#define HEAP_SIZE (16ULL * 1024 * 1024)

/**
 * First virtual address past the end of the heap (exclusive).
 */
#define HEAP_END (HEAP_START + HEAP_SIZE)

/**
 * Initializes the kernel heap.
 *
 * Maps the first page at HEAP_START and turns it into a single free
 * block. The heap then grows on demand, one page at a time, as
 * k_malloc() needs more space.
 *
 * Must be called once, after pmm_init() and paging_init(), and
 * before any k_malloc()/k_free() call. If a physical page cannot be
 * allocated or mapped, it returns silently without initializing the
 * heap; no error is reported.
 */
void heap_init(void);

/**
 * Allocates a block of memory from the kernel heap.
 *
 * Uses a best-fit search over the free blocks: the smallest free
 * block that fits is chosen, and split if the remainder is large
 * enough to hold another block. If no block fits, the heap is grown
 * one page at a time until one does.
 *
 * The size is rounded up to a multiple of 8, and the returned
 * pointer is 8-byte aligned. The memory is not zeroed.
 *
 * No locking is done, so this is not safe to call concurrently
 * (e.g. from both regular code and an IRQ handler).
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to the allocated memory, or NULL if size is 0, the
 *         heap reached HEAP_SIZE, or no physical page was available
 *         to grow it.
 */
void* k_malloc(u64 size);

/**
 * Frees a block previously returned by k_malloc().
 *
 * The block is merged with its neighbors if they are also free. If
 * the resulting free block is the last one in the heap, the whole
 * pages beyond the one holding its header are unmapped and returned
 * to the PMM, shrinking the heap. The first heap page is never
 * released.
 *
 * Does nothing if ptr is NULL, lies outside the mapped heap, does
 * not point to a valid block (header magic mismatch), or points to a
 * block that is already free (double free).
 *
 * No locking is done, so this is not safe to call concurrently
 * (see k_malloc).
 *
 * @param ptr Pointer returned by k_malloc().
 */
void k_free(void* ptr);

#endif