#include <memory/paging.h>
#include <memory/memory.h>
#include <memory/pmm.h>
#include <math/math.h>
#include <log/log.h>

typedef enum {
    PAGING_WALK_NOT_MAPPED,
    PAGING_WALK_PAGE,
    PAGING_WALK_HUGE
} PagingWalkType;

static PageTable* pml4 = NULL;
static bool direct_map_active = false;

static bool paging_init_direct_map(void);
static void paging_invalidate_page(u64 virtual_address);

static u64 paging_pml4_index(u64 virtual_address);
static u64 paging_pdpt_index(u64 virtual_address);
static u64 paging_pd_index(u64 virtual_address);
static u64 paging_pt_index(u64 virtual_address);

static bool paging_translate(u64 virtual_address, u64* physical_address);
static bool paging_map_page(u64 virtual_address, u64 physical_address, u64 flags);
static bool paging_map_huge_page(u64 virtual_address, u64 physical_address, u64 flags);
static bool paging_unmap_page(u64 virtual_address);

static bool paging_set_flags(u64 virtual_address, u64 flags);
static bool paging_get_flags(u64 virtual_address, u64* flags);

static PageTable* paging_create_table(PageTableEntry* entry);
static PageTable* paging_get_table(PageTableEntry entry);

static PagingWalkType paging_walk(u64 virtual_address, PageTableEntry** entry);

static u64 align_up(u64 value, u64 alignment);
static u64 align_down(u64 value, u64 alignment);

void paging_init(void) {
    u64 pml4_page = pmm_alloc_page();
    pml4 = (PageTable*)pml4_page;
    mem_set(pml4, 0x00, PAGE_SIZE);

    u64 pdpt_page = pmm_alloc_page();
    PageTable* pdpt = (PageTable*)pdpt_page;
    mem_set(pdpt, 0x00, PAGE_SIZE);

    pml4[0][0] = pdpt_page | PAGE_PRESENT | PAGE_WRITABLE;

    u64 pd_page = pmm_alloc_page();
    PageTable* pd = (PageTable*)pd_page;
    mem_set(pd, 0x00, PAGE_SIZE);

    pdpt[0][0] = pd_page | PAGE_PRESENT | PAGE_WRITABLE;

    u64 pt_page = pmm_alloc_page();
    PageTable* pt = (PageTable*)pt_page;
    mem_set(pt, 0x00, PAGE_SIZE);

    pd[0][0] = pt_page | PAGE_PRESENT | PAGE_WRITABLE;

    for (u64 i = 0; i < 512; i++) {
        pt[0][i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    paging_init_direct_map();

    asm volatile (
        "mov %0, %%cr3"
        :
        : "r"(pml4_page)
        : "memory"
    );

    direct_map_active = true;

    u64 virtual_pml4_page_address;
    if (!physical_to_virtual(pml4_page, &virtual_pml4_page_address)) {
        return;
    }

    pml4 = (PageTable*)virtual_pml4_page_address;
}

bool physical_to_virtual(u64 physical_address, u64* virtual_address) {
    if (virtual_address == NULL) {
        return false;
    }

    *virtual_address = DIRECT_MAP_BASE + physical_address;
    return true;
}

bool virtual_to_physical(u64 virtual_address, u64* physical_address) {
    if (physical_address == NULL) {
        return false;
    }

    if (virtual_address < DIRECT_MAP_BASE) {
        return false;
    }

    *physical_address = virtual_address - DIRECT_MAP_BASE;
    return true;
}

static bool paging_init_direct_map(void) {
    bool ok = true;

    u64 virtual_address;
    if (!physical_to_virtual(0x0, &virtual_address)) {
        return false;
    }

    if (!paging_map_huge_page(virtual_address, 0x0, PAGE_PRESENT | PAGE_WRITABLE)) {
        ok = false;
    }

    u64 region_count = pmm_get_region_count();
    for (u64 i = 0; i < region_count; i++) {
        MemoryRegion* region = pmm_get_memory_region(i);
        u64 region_end = region->start + (region->page_count * PAGE_SIZE);

        u64 aligned_start = align_up(region->start, HUGE_PAGE_SIZE);
        u64 aligned_end = align_down(region_end, HUGE_PAGE_SIZE);

        u64 leading_end = aligned_start < region_end ? aligned_start : region_end;

        for (u64 page = region->start; page < leading_end; page += PAGE_SIZE) {
            if (!physical_to_virtual(page, &virtual_address) ||
                !paging_map_page(virtual_address, page, PAGE_PRESENT | PAGE_WRITABLE)) {
                ok = false;
            }
        }

        if (aligned_start < aligned_end) {
            for (u64 block = aligned_start; block < aligned_end; block += HUGE_PAGE_SIZE) {
                if (block == 0x0) {
                    continue;
                }

                if (!physical_to_virtual(block, &virtual_address) ||
                    !paging_map_huge_page(virtual_address, block, PAGE_PRESENT | PAGE_WRITABLE)) {
                    ok = false;
                }
            }

            for (u64 page = aligned_end; page < region_end; page += PAGE_SIZE) {
                if (!physical_to_virtual(page, &virtual_address) ||
                    !paging_map_page(virtual_address, page, PAGE_PRESENT | PAGE_WRITABLE)) {
                    ok = false;
                }
            }
        }
    }

    return ok;
}

static void paging_invalidate_page(u64 virtual_address) {
    asm volatile (
        "invlpg (%0)"
        :
        : "r"(virtual_address)
        : "memory"
    );
}

static u64 paging_pml4_index(u64 virtual_address) {
    return (virtual_address >> 39) & 0x1FF;
}

static u64 paging_pdpt_index(u64 virtual_address) {
    return (virtual_address >> 30) & 0x1FF;
}

static u64 paging_pd_index(u64 virtual_address) {
    return (virtual_address >> 21) & 0x1FF;
}

static u64 paging_pt_index(u64 virtual_address) {
    return (virtual_address >> 12) & 0x1FF;
}

static bool paging_translate(u64 virtual_address, u64* physical_address) {
    if (physical_address == NULL) {
        return false;
    }

    PageTableEntry* entry;
    PagingWalkType type = paging_walk(virtual_address, &entry);

    if (type == PAGING_WALK_NOT_MAPPED) {
        return false;
    }

    u64 physical_base = type == PAGING_WALK_HUGE
                        ? (*entry & ~(HUGE_PAGE_SIZE - 1))
                        : (*entry & ~(PAGE_SIZE - 1));
    
    u64 offset = type == PAGING_WALK_HUGE
                 ? (virtual_address & (HUGE_PAGE_SIZE - 1))
                 : (virtual_address & (PAGE_SIZE - 1));

    *physical_address = physical_base + offset;

    return true;
}

static bool paging_map_page(u64 virtual_address, u64 physical_address, u64 flags) {
    if (physical_address % PAGE_SIZE != 0) {
        return false;
    }

    if (virtual_address % PAGE_SIZE != 0) {
        return false;
    }
    
    u64 pml4_index = paging_pml4_index(virtual_address);
    PageTable* pdpt = paging_create_table(&pml4[0][pml4_index]);
    if (pdpt == NULL) {
        return false;
    }

    u64 pdpt_index = paging_pdpt_index(virtual_address);
    PageTable* pd = paging_create_table(&pdpt[0][pdpt_index]);
    if (pd == NULL) {
        return false;
    }

    u64 pd_index = paging_pd_index(virtual_address);
    if (pd[0][pd_index] & PAGE_HUGE) {
        return false;
    }

    PageTable* pt = paging_create_table(&pd[0][pd_index]);
    if (pt == NULL) {
        return false;
    }

    u64 pt_index = paging_pt_index(virtual_address);
    if (pt[0][pt_index] != 0) {
        return false;
    }
    
    pt[0][pt_index] = physical_address | flags;
    return true;
}

static bool paging_map_huge_page(u64 virtual_address, u64 physical_address, u64 flags) {
    if (physical_address % HUGE_PAGE_SIZE != 0) {
        return false;
    }

    if (virtual_address % HUGE_PAGE_SIZE != 0) {
        return false;
    }

    u64 pml4_index = paging_pml4_index(virtual_address);
    PageTable* pdpt = paging_create_table(&pml4[0][pml4_index]);
    if (pdpt == NULL) {
        return false;
    }

    u64 pdpt_index = paging_pdpt_index(virtual_address);
    PageTable* pd = paging_create_table(&pdpt[0][pdpt_index]);
    if (pd == NULL) {
        return false;
    }

    u64 pd_index = paging_pd_index(virtual_address);
    if (pd[0][pd_index] != 0) {
        return false;
    }

    pd[0][pd_index] = physical_address | flags | PAGE_HUGE;

    return true;
}

static bool paging_unmap_page(u64 virtual_address) {
    if (virtual_address % PAGE_SIZE != 0) {
        return false;
    }

    PageTableEntry* entry;
    PagingWalkType type = paging_walk(virtual_address, &entry);
    if (type == PAGING_WALK_NOT_MAPPED || type == PAGING_WALK_HUGE) {
        return false;
    }

    *entry = 0;
    paging_invalidate_page(virtual_address);

    return true;
}

static bool paging_set_flags(u64 virtual_address, u64 flags) {
    PageTableEntry* entry;
    PagingWalkType type = paging_walk(virtual_address, &entry);

    if (type == PAGING_WALK_NOT_MAPPED) {
        return false;
    }

    u64 physical_base = type == PAGING_WALK_HUGE
                        ? (*entry & ~(HUGE_PAGE_SIZE - 1))
                        : (*entry & ~(PAGE_SIZE - 1));

    *entry = physical_base | flags | 
             (type == PAGING_WALK_HUGE 
                ? PAGE_HUGE 
                : 0
             );

    paging_invalidate_page(virtual_address);
    return true;
}

static bool paging_get_flags(u64 virtual_address, u64* flags) {
    if (flags == NULL) {
        return false;
    }
    
    PageTableEntry* entry;
    PagingWalkType type = paging_walk(virtual_address, &entry);
    if (type == PAGING_WALK_NOT_MAPPED) {
        return false;
    }

    *flags = *entry & 0xFFF;
    return true;
}

static PageTable* paging_create_table(PageTableEntry* entry) {
    if (*entry != 0) {
        return paging_get_table(*entry);
    }

    u64 address = pmm_alloc_page();
    if (address == 0) {
        return NULL;
    }

    PageTable* table;
    if (direct_map_active) {
        u64 virtual_address;

        if (!physical_to_virtual(address, &virtual_address)) {
            pmm_free_page(address);
            return NULL;
        }

        table = (PageTable*)virtual_address;
    } else {
        table = (PageTable*)address;
    }

    mem_set(table, 0x0, PAGE_SIZE);

    *entry = address | PAGE_PRESENT | PAGE_WRITABLE;
    return table;
}

static PageTable* paging_get_table(PageTableEntry entry) {
    u64 physical = entry & ~0xFFF;

    if (!direct_map_active) {
        return (PageTable*)physical;
    }

    u64 virtual_address;
    if (!physical_to_virtual(physical, &virtual_address)) {
        return NULL;
    }

    return (PageTable*)virtual_address;
}

static PagingWalkType paging_walk(u64 virtual_address, PageTableEntry** entry) {
    if (entry == NULL) {
        return PAGING_WALK_NOT_MAPPED;
    }

    u64 pml4_index = paging_pml4_index(virtual_address);
    if (pml4[0][pml4_index] == 0) {
        return PAGING_WALK_NOT_MAPPED;
    }

    PageTable* pdpt = paging_get_table(pml4[0][pml4_index]);
    if (pdpt == NULL) {
        return PAGING_WALK_NOT_MAPPED;
    }

    u64 pdpt_index = paging_pdpt_index(virtual_address);
    if (pdpt[0][pdpt_index] == 0) {
        return PAGING_WALK_NOT_MAPPED;
    }

    PageTable* pd = paging_get_table(pdpt[0][pdpt_index]);
    if (pd == NULL) {
        return PAGING_WALK_NOT_MAPPED;
    }

    u64 pd_index = paging_pd_index(virtual_address);
    if (pd[0][pd_index] == 0) {
        return PAGING_WALK_NOT_MAPPED;
    }

    if (pd[0][pd_index] & PAGE_HUGE) {
        *entry = &pd[0][pd_index];
        return PAGING_WALK_HUGE;
    }

    PageTable* pt = paging_get_table(pd[0][pd_index]);
    if (pt == NULL) {
        return PAGING_WALK_NOT_MAPPED;
    }

    u64 pt_index = paging_pt_index(virtual_address);
    if (pt[0][pt_index] == 0) {
        return PAGING_WALK_NOT_MAPPED;
    }

    *entry = &pt[0][pt_index];
    return PAGING_WALK_PAGE;
}

static u64 align_up(u64 value, u64 alignment) {
    return ceil_div(value, alignment) * alignment;
}

static u64 align_down(u64 value, u64 alignment) {
    return (value / alignment) * alignment;
}