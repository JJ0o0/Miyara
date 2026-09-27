#include <memory/pmm.h>
#include <math/math.h>

#define PAGE_SIZE 0x1000
#define MAX_MEMORY_REGIONS 32
static MemoryRegion memory_regions[MAX_MEMORY_REGIONS] = {0};
static u64 region_count = 0;

static u8 bitmap_region0[PAGE_SIZE] = {0};
static u8 bitmap_region1[PAGE_SIZE] = {0};

static void bitmap_init(MemoryRegion* region);
static void bitmap_release_region(MemoryRegion* region);
static void bitmap_reserve_page(u64 physical_address);
static void bitmap_reserve_range(u64 start, u64 length);
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

    bitmap_reserve_range(0x100000, 0xB000); // KERNEL
    bitmap_reserve_range(0x10B000, 0x1000); // MBI
}

u64 pmm_alloc_page(void) {
    for (u64 i = 0; i < region_count; i++) {
        u64 physical_address = bitmap_find_free_page(&memory_regions[i]);
        if (physical_address != 0) {
            bitmap_reserve_page(physical_address);
            return physical_address;
        }
    }

    return 0;
}

void pmm_free_page(u64 physical_address) {
    MemoryRegion* mr = find_memory_region(physical_address);
    if (mr == NULL) {
        return;
    }

    u64 page_index = physical_to_page_index(mr, physical_address);
    u64 byte_index = page_index / 8;
    u64 bit_index = page_index % 8;
    mr->bitmap[byte_index] &= ~(1ULL << bit_index);
}

static void bitmap_init(MemoryRegion* region) {
    u64 bitmap_bytes = ceil_div(region->page_count, 8);
    for (u64 i = 0; i < bitmap_bytes; i++) {
        region->bitmap[i] = 0xFF;
    }
}

static void bitmap_release_region(MemoryRegion* region) {
    u64 bitmap_bytes = ceil_div(region->page_count, 8);
    u64 remaining_bits = region->page_count % 8;
    for (u64 i = 0; i < bitmap_bytes; i++) {
        region->bitmap[i] = ~region->bitmap[i];
    }

    if (remaining_bits != 0) {
        region->bitmap[bitmap_bytes - 1] = ~((1ULL << remaining_bits) - 1);
    }
}

static void bitmap_reserve_page(u64 physical_address) {
    MemoryRegion* mr = find_memory_region(physical_address);
    if (mr == NULL) {
        return;
    }

    u64 page_index = physical_to_page_index(mr, physical_address);
    u64 byte_index = page_index / 8;
    u64 bit_index = page_index % 8;
    mr->bitmap[byte_index] |= (1ULL << bit_index);
}

static void bitmap_reserve_range(u64 start, u64 length) {
    u64 page_count = length / PAGE_SIZE;
    for (u64 i = 0; i < page_count; i++) {
        u64 physical_address = start + (i * PAGE_SIZE);
        bitmap_reserve_page(physical_address);
    }
}

static u64 bitmap_find_free_page(MemoryRegion* region) {
    for (u64 page_index = 0; page_index < region->page_count; page_index++) {
        u64 byte_index = page_index / 8;
        u64 bit_index = page_index % 8;

        if ((region->bitmap[byte_index] & (1ULL << bit_index)) == 0) {
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
    u64 byte_index = page_index / 8;
    u64 bit_index = page_index % 8;
    return mr->bitmap[byte_index] & (1ULL << bit_index);
}