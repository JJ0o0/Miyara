#include <memory/heap.h>

#include <memory/memory.h>
#include <memory/paging.h>
#include <memory/pmm.h>

#include <types/types.h>
#include <math/math.h>

#define HEAP_MAGIC 0x4D49594152414845ULL

#define MIN_BLOCK_PAYLOAD 8
#define BLOCK_FREE 0
#define BLOCK_USED 1

typedef struct BlockHeader {
    u64 magic;
    u64 size;
    u8 free;
    struct BlockHeader* next;
    struct BlockHeader* prev;
} BlockHeader;

static u64 heap_mapped_end;

static bool heap_grow(void);
static u64 heap_get_reclaim_start(BlockHeader* block);
static BlockHeader* find_best_block(u64 size);

void heap_init(void) {
    u64 physical_page = pmm_alloc_page();
    if (physical_page == 0) {
        return;
    }

    if (!paging_map_page(
        HEAP_START, 
        physical_page, 
        PAGE_PRESENT | PAGE_WRITABLE
    )) {
        pmm_free_page(physical_page);
        return;
    }

    BlockHeader* heap_start_bh = (BlockHeader*)HEAP_START;
    heap_start_bh->magic = HEAP_MAGIC;
    heap_start_bh->size = PAGE_SIZE - sizeof(BlockHeader);
    heap_start_bh->free = BLOCK_FREE;
    heap_start_bh->next = NULL;
    heap_start_bh->prev = NULL;

    heap_mapped_end = HEAP_START + PAGE_SIZE;
}

void* k_malloc(u64 size) {
    if (size == 0 || size > HEAP_SIZE - sizeof(BlockHeader)) {
        return NULL;
    }

    u64 aligned = align_up(size, 8);

    BlockHeader* best_block = find_best_block(aligned);
    if (best_block == NULL) {
        while (best_block == NULL) {
            if (!heap_grow()) {
                return NULL;
            }

            best_block = find_best_block(aligned);
        }
    }

    u64 rest = best_block->size - aligned;
    if (rest >= sizeof(BlockHeader) + MIN_BLOCK_PAYLOAD) {
        BlockHeader* new_block = (BlockHeader*)((u8*)best_block + sizeof(BlockHeader) + aligned);
        new_block->magic = HEAP_MAGIC;
        new_block->size = best_block->size - sizeof(BlockHeader) - aligned;
        new_block->free = BLOCK_FREE;
        new_block->prev = best_block;
        new_block->next = best_block->next;

        if (new_block->next != NULL) {
            new_block->next->prev = new_block;
        }

        best_block->next = new_block;
        best_block->size = aligned;
    }

    best_block->free = BLOCK_USED;
    return (void*)(best_block + 1);
}

void k_free(void* ptr) {
    if (ptr == NULL) {
        return;
    } 

    u64 address = (u64)ptr;
    if (address < HEAP_START || address >= heap_mapped_end) {
        return;
    }

    BlockHeader* block = (BlockHeader*)(address - sizeof(BlockHeader));
    if (block->magic != HEAP_MAGIC) {
        return;
    }

    if (block->free != BLOCK_USED) {
        return;
    }

    block->free = BLOCK_FREE;
    if (block->prev != NULL && block->prev->free == BLOCK_FREE) {
        BlockHeader* prev = block->prev;
        prev->size += sizeof(BlockHeader) + block->size;
        prev->next = block->next;

        if (block->next != NULL) {
            block->next->prev = prev;
        }

        block = prev;
    }

    if (block->next != NULL && block->next->free == BLOCK_FREE) {
        BlockHeader* next = block->next;
        block->size += sizeof(BlockHeader) + next->size;
        block->next = next->next;

        if (next->next != NULL) {
            next->next->prev = block;
        }
    }

    if (block->next != NULL) {
        return;
    }

    u64 reclaim_start = heap_get_reclaim_start(block);
    if (reclaim_start == heap_mapped_end) {
        return;
    }

    for (u64 page = reclaim_start; page < heap_mapped_end; page += PAGE_SIZE) {
        u64 physical_page;
        if (!virtual_to_physical(page, &physical_page)) {
            return;
        }

        if (!paging_unmap_page(page)) {
            return;
        }

        pmm_free_page(physical_page);
    }

    heap_mapped_end = reclaim_start;
    block->size = reclaim_start - (u64)block - sizeof(BlockHeader);
}

static bool heap_grow(void) {
    if (heap_mapped_end + PAGE_SIZE > HEAP_END) {
        return false;
    }

    u64 physical_page = pmm_alloc_page();
    if (physical_page == 0) {
        return false;
    }

    if (!paging_map_page(
        heap_mapped_end, 
        physical_page, 
        PAGE_PRESENT | PAGE_WRITABLE
    )) {
        pmm_free_page(physical_page);
        return false;
    }

    BlockHeader* new_block = (BlockHeader*)heap_mapped_end;
    new_block->magic = HEAP_MAGIC;
    new_block->size = PAGE_SIZE - sizeof(BlockHeader);
    new_block->free = BLOCK_FREE;

    BlockHeader* last_block = (BlockHeader*)HEAP_START;
    while (last_block->next != NULL) {
        last_block = last_block->next;
    }

    if (last_block->free != BLOCK_FREE) {
        new_block->prev = last_block;
        new_block->next = NULL;
        last_block->next = new_block;
    } else {
        last_block->size += sizeof(BlockHeader) + new_block->size;
    }

    heap_mapped_end += PAGE_SIZE;
    return true;
}

static u64 heap_get_reclaim_start(BlockHeader* block) {
    u64 block_start = (u64)block;

    u64 reclaim_start = align_up(block_start, PAGE_SIZE);
    if (reclaim_start == block_start) {
        reclaim_start += PAGE_SIZE;
    }

    u64 min_reclaim_start = HEAP_START + PAGE_SIZE;
    if (reclaim_start < min_reclaim_start) {
        reclaim_start = min_reclaim_start;
    }

    if (reclaim_start >= heap_mapped_end) {
        return heap_mapped_end;
    }

    return reclaim_start;
}

static BlockHeader* find_best_block(u64 size) {
    BlockHeader* current = (BlockHeader*)HEAP_START;
    BlockHeader* best_block = NULL;
    while (current != NULL) {
        if (current->free == BLOCK_FREE && current->size >= size) {
            if (best_block == NULL) {
                best_block = current;
            } else if (current->size < best_block->size) {
                best_block = current;
            }
        }

        current = current->next;
    }

    return best_block;
}