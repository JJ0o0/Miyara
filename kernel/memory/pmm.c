#include <memory/pmm.h>
#include <math/math.h>
#include <log/log.h>

extern u8 kernel_start;
extern u8 kernel_end;

#define PAGE_STATE_BITS 2
#define PAGE_STATES_PER_BYTE (8 / PAGE_STATE_BITS)
#define MAX_MEMORY_REGIONS 32
static MemoryRegion memory_regions[MAX_MEMORY_REGIONS] = {0};
static u64 region_count = 0;

static u8 bitmap_region0[PAGE_SIZE] = {0};
static u8 bitmap_region1[PAGE_SIZE * 2] = {0};

typedef enum {
    PAGE_FREE,
    PAGE_ALLOCATED,
    PAGE_RESERVED,
    PAGE_INVALID
} PageState;

static void bitmap_init(MemoryRegion* region);
static void bitmap_release_region(MemoryRegion* region);

static void bitmap_alloc_page(u64 physical_address);
static void bitmap_reserve_page(u64 physical_address);
static void bitmap_reserve_range(u64 start, u64 length);

static void bitmap_set_page_state(MemoryRegion* region, u64 page_index, PageState state);
static PageState bitmap_get_page_state(MemoryRegion* region, u64 page_index);

static u64 bitmap_find_free_page(MemoryRegion* region);
static u64 physical_to_page_index(MemoryRegion* region, u64 physical_address);
static MemoryRegion* find_memory_region(u64 physical_address);

// DEBUG
static bool bitmap_is_page_reserved(u64 physical_address);

void pmm_init(MBIHeader* mbi) {
    TagHeader* current_tag = (TagHeader*)((u8*)mbi + 0x8);
    MemoryMapTag* mm_tag;
    bool found_mm = false;
    while (current_tag->type != 0) {
        if (current_tag->type == 0x6) {
            mm_tag = (MemoryMapTag*)current_tag;
            found_mm = true;
            break;
        }

        u8* next = (u8*)current_tag + current_tag->size;
        u8 left = (u64)next % 8;

        u8 offset = 0;
        if (left != 0) {
            offset = 8 - left;
        }

        current_tag = (TagHeader*)(next + offset);
    }

    if (!found_mm) {
        return;
    }

    u32 entry_bytes = mm_tag->size - sizeof(MemoryMapTag);
    u32 entry_quantity = entry_bytes / mm_tag->entry_size;
    
    MemoryMapEntry* mm_entry = (MemoryMapEntry*)((u8*)mm_tag + sizeof(MemoryMapTag));
    for (u32 i = 0; i < entry_quantity; i++)  {
        if (mm_entry->type == 0x1) {
            if (region_count >= MAX_MEMORY_REGIONS) {
                continue;
            }

            memory_regions[region_count].start = mm_entry->addr;
            memory_regions[region_count].length = mm_entry->len;
            memory_regions[region_count].page_count = mm_entry->len / PAGE_SIZE;

            if (region_count == 0) {
                memory_regions[region_count].bitmap = bitmap_region0;
            } else if (region_count == 1) {
                memory_regions[region_count].bitmap = bitmap_region1;
            }

            bitmap_init(&memory_regions[region_count]);
            bitmap_release_region(&memory_regions[region_count]);

            region_count++;
        }

        mm_entry = (MemoryMapEntry*)((u8*)mm_entry + mm_tag->entry_size);
    }

    u64 kernel_start_address = (u64)&kernel_start;
    u64 kernel_end_address = (u64)&kernel_end;

    u64 kernel_size = kernel_end_address - kernel_start_address;
    u64 kernel_reserved_size = ceil_div(kernel_size, PAGE_SIZE) * PAGE_SIZE;

    bitmap_reserve_range(kernel_start_address, kernel_reserved_size); // KERNEL
    bitmap_reserve_range((u64)mbi, mbi->total_size); // MBI
}

u64 pmm_alloc_page(void) {
    for (u64 i = 0; i < region_count; i++) {
        u64 physical_address = bitmap_find_free_page(&memory_regions[i]);
        if (physical_address != 0) {
            bitmap_alloc_page(physical_address);
            return physical_address;
        }
    }

    return 0;
}

void pmm_free_page(u64 physical_address) {
    if (physical_address % PAGE_SIZE != 0) {
        return;
    }

    MemoryRegion* mr = find_memory_region(physical_address);
    if (mr == NULL) {
        return;
    }

    u64 page_index = physical_to_page_index(mr, physical_address);
    if (bitmap_get_page_state(mr, page_index) != PAGE_ALLOCATED) {
        return;
    }

    bitmap_set_page_state(mr, page_index, PAGE_FREE);
}

static void bitmap_init(MemoryRegion* region) {
    u64 bitmap_bytes = ceil_div(region->page_count, PAGE_STATES_PER_BYTE);
    for (u64 i = 0; i < bitmap_bytes; i++) {
        region->bitmap[i] = 0xFF;
    }
}

static void bitmap_release_region(MemoryRegion* region) {
    for (u64 page_index = 0; page_index < region->page_count; page_index++) {
        bitmap_set_page_state(region, page_index, PAGE_FREE);
    }
}

static void bitmap_alloc_page(u64 physical_address) {
    MemoryRegion* mr = find_memory_region(physical_address);
    if (mr == NULL) {
        return;
    }

    u64 page_index = physical_to_page_index(mr, physical_address);
    if (bitmap_get_page_state(mr, page_index) != PAGE_FREE) {
        return;
    }

    bitmap_set_page_state(mr, page_index, PAGE_ALLOCATED);
}

static void bitmap_reserve_page(u64 physical_address) {
    MemoryRegion* mr = find_memory_region(physical_address);
    if (mr == NULL) {
        return;
    }

    u64 page_index = physical_to_page_index(mr, physical_address);
    bitmap_set_page_state(mr, page_index, PAGE_RESERVED);
}

static void bitmap_reserve_range(u64 start, u64 length) {
    if (length == 0) {
        return;
    }

    u64 start_page = start / PAGE_SIZE;
    u64 end_page = (start + length - 1) / PAGE_SIZE;
    for (u64 page = start_page; page <= end_page; page++) {
        u64 physical_address = page * PAGE_SIZE;
        bitmap_reserve_page(physical_address);
    }
}

static void bitmap_set_page_state(MemoryRegion* region, u64 page_index, PageState state) {
    u64 byte_index = page_index / PAGE_STATES_PER_BYTE;
    u64 bit_offset = (page_index % PAGE_STATES_PER_BYTE) * PAGE_STATE_BITS;
    region->bitmap[byte_index] &= ~(0b11 << bit_offset);
    region->bitmap[byte_index] |= ((state & 0b11) << bit_offset);
}

static PageState bitmap_get_page_state(MemoryRegion* region, u64 page_index) {
    u64 byte_index = page_index / PAGE_STATES_PER_BYTE;
    u64 bit_offset = (page_index % PAGE_STATES_PER_BYTE) * PAGE_STATE_BITS;
    return (PageState)((region->bitmap[byte_index] >> bit_offset) & 0b11);
}

static u64 bitmap_find_free_page(MemoryRegion* region) {
    for (u64 page_index = 0; page_index < region->page_count; page_index++) {
        if (bitmap_get_page_state(region, page_index) == PAGE_FREE) {
            return region->start + (page_index * PAGE_SIZE);
        }
    }

    return 0;
}

static u64 physical_to_page_index(MemoryRegion* region, u64 physical_address) {
    return (physical_address - region->start) / PAGE_SIZE;
}

static MemoryRegion* find_memory_region(u64 physical_address) {
    MemoryRegion* mr = NULL;
    for (u8 i = 0; i < region_count; i++) {
        MemoryRegion* region = &memory_regions[i];
        if (
            physical_address >= region->start 
            && physical_address < region->start + region->length
        ) {
            mr = region;
            break;
        }
    }

    return mr;
}

// DEBUG
static bool bitmap_is_page_reserved(u64 physical_address) {
    MemoryRegion* mr = find_memory_region(physical_address);
    if (mr == NULL) {
        return false;
    }

    u64 page_index = physical_to_page_index(mr, physical_address);
    return bitmap_get_page_state(mr, page_index) == PAGE_RESERVED;
}