#include <memory/memory.h>
#include <memory/paging.h>
#include <memory/pmm.h>

static PageTable* pml4 = NULL;

void paging_init(void) {
    u64 plm4_page = pmm_alloc_page();
    pml4 = (PageTable*)plm4_page;
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

    asm volatile (
        "mov %0, %%cr3"
        :
        : "r"(plm4_page)
        : "memory"
    );
}