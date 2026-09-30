#ifndef PAGING_H
#define PAGING_H

#include <types/types.h>

/**
 * Virtual base address of the direct physical memory map.
 *
 * Once paging_init() finishes, every physical address `p` that has
 * been direct-mapped is also accessible at DIRECT_MAP_BASE + p (see
 * physical_to_virtual). Chosen in the canonical higher half of the
 * 64-bit address space, well away from any future kernel image or
 * heap mappings.
 */
#define DIRECT_MAP_BASE 0xFFFF800000000000ULL

/**
 * Size of a huge (2MB) page, in bytes. A huge page is mapped at the
 * Page Directory level, skipping the Page Table level entirely.
 */
#define HUGE_PAGE_SIZE 0x200000

/**
 * Page/entry flag: this Page Directory entry maps a 2MB huge page
 * instead of pointing to a Page Table (the PS bit).
 */
#define PAGE_HUGE     0x80

/**
 * Page table entry flag: the entry is present (valid).
 */
#define PAGE_PRESENT  0x1

/**
 * Page table entry flag: the mapped region is writable.
 */
#define PAGE_WRITABLE 0x2

/**
 * Page table entry flag: the mapped region is accessible from ring 3
 * (user mode).
 */
#define PAGE_USER     0x4

/**
 * A single page table entry: a physical address (bits 12-51) plus
 * flag bits (bits 0-11, and PAGE_HUGE at the PD level).
 */
typedef u64 PageTableEntry;

/**
 * One level of the page table hierarchy: 512 entries, exactly one
 * 4KB page. Used for the PML4, PDPT, PD and PT alike.
 */
typedef PageTableEntry PageTable[512];

/**
 * Initializes paging.
 *
 * Builds a minimal PML4/PDPT/PD/PT hierarchy that identity-maps the
 * first 2MB (needed to keep executing after switching CR3, since the
 * kernel itself lives there), then builds the direct physical memory
 * map (see physical_to_virtual) by huge-mapping every 2MB-aligned
 * block of every region known to the PMM. Finally loads CR3 with the
 * new PML4 and switches all further page-table access to go through
 * the direct map.
 *
 * Must be called once, after pmm_init(), since it allocates page
 * table pages via pmm_alloc_page().
 */
void paging_init(void);

/**
 * Converts a physical address to its direct-mapped virtual address.
 *
 * Only valid for physical addresses that are actually covered by the
 * direct map built in paging_init() — physical memory outside every
 * registered region, or inside an unmapped gap of a region (see
 * paging_init_direct_map), is not guaranteed to be mapped, and
 * dereferencing the result will fault.
 *
 * @param physical_address Physical address to convert.
 * @param virtual_address Output parameter; filled with the
 *                         corresponding virtual address on success.
 * @return true on success, false if virtual_address is NULL.
 */
bool physical_to_virtual(u64 physical_address, u64* virtual_address);

/**
 * Converts a direct-mapped virtual address back to its physical
 * address.
 *
 * Only valid for addresses previously obtained from
 * physical_to_virtual (or otherwise known to lie in the direct map,
 * i.e. >= DIRECT_MAP_BASE).
 *
 * @param virtual_address Direct-mapped virtual address to convert.
 * @param physical_address Output parameter; filled with the
 *                          corresponding physical address on success.
 * @return true on success, false if physical_address is NULL or
 *         virtual_address is below DIRECT_MAP_BASE.
 */
bool virtual_to_physical(u64 virtual_address, u64* physical_address);
#endif