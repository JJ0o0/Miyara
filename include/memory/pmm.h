#ifndef PMM_H
#define PMM_H

#include <multiboot/multiboot.h>
#include <types/types.h>

/**
 * A contiguous, page-aligned range of physical memory tracked by the
 * PMM, together with its own allocation bitmap.
 *
 * Each region owns a separate bitmap buffer (statically allocated per
 * region index in pmm.c), sized to fit that region's page_count. Every
 * page's state is stored in bitmap using PAGE_STATE_BITS bits (see
 * PageState in pmm.c).
 */
typedef struct {
    /**
     * Physical start address of the region. Page-aligned.
     */
    u64 start;

    /**
     * Length of the region, in bytes.
     */
    u64 length;

    /**
     * Number of PAGE_SIZE pages the region contains
     * (length / PAGE_SIZE).
     */
    u64 page_count;

    /**
     * Per-page state bitmap for this region. Indexed by page index
     * (physical_address - start) / PAGE_SIZE, PAGE_STATE_BITS bits
     * per page.
     */
    u8* bitmap;
} MemoryRegion;

/**
 * Initializes the physical memory manager from the Multiboot2 memory
 * map.
 *
 * Walks the MBI's tags to find the memory map tag, registers each
 * available (type == 1) region up to MAX_MEMORY_REGIONS, builds each
 * region's bitmap (initially all pages marked free), then reserves
 * the physical ranges used by the kernel image and by the MBI itself
 * so they are never handed out by pmm_alloc_page.
 *
 * Must be called once, before any pmm_alloc_page/pmm_free_page call.
 * If no memory map tag is found, the PMM is left with zero regions
 * and every allocation will fail.
 *
 * @param mbi Pointer to the Multiboot2 info structure (as passed to
 *            the kernel by the bootloader).
 */
void pmm_init(MBIHeader* mbi);

/**
 * Allocates a single free physical page.
 *
 * Scans registered regions in order and returns the first free page
 * found, marking it allocated.
 *
 * @return Physical address of the allocated page (page-aligned), or
 *         0 if no free page is available.
 */
u64 pmm_alloc_page(void);

/**
 * Frees a previously allocated physical page.
 *
 * No-op if physical_address is not page-aligned, falls outside every
 * registered region, or is not currently marked as allocated (e.g.
 * freeing a reserved or already-free page).
 *
 * @param physical_address Physical address of the page to free
 *                          (must be page-aligned).
 */
void pmm_free_page(u64 physical_address);

#endif