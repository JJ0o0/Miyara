#include <process/process.h>
#include <scheduler/scheduler.h>
#include <panic/panic.h>

#include <memory/memory.h>
#include <memory/paging.h>
#include <memory/heap.h>
#include <memory/pmm.h>

#include <types/types.h>
#include <timer/timer.h>
#include <math/math.h>

#define USER_CODE_BASE  0x0000000000400000ULL
#define USER_STACK_TOP  0x0000000040000000ULL

static u64 next_pid = 1;

static bool process_map_user_stack(u64 user_stack_base);
static bool process_map_user_code(u64 user_code_address);

Process* process_create(void (*entry)(void)) {
    if (entry == NULL) {
        return NULL;
    }

    Process* process = k_malloc(sizeof(Process));
    if (process == NULL) {
        return NULL;
    }

    void* stack_base = k_malloc(PROCESS_STACK_SIZE);
    if (stack_base == NULL) {
        k_free(process);
        return NULL;
    }

    u64 stack_end = (u64)stack_base + PROCESS_STACK_SIZE;
    stack_end = align_down(stack_end, 16);

    u64 task_rsp = stack_end - 8;
    *(u64*)task_rsp = 0;

    CPUContext* context = (CPUContext*)(task_rsp - sizeof(CPUContext));
    mem_set(context, 0, sizeof(CPUContext));

    context->rip = (u64)process_trampoline;
    context->cs = 0x18;
    context->rflags = 0x202;
    context->rsp = task_rsp;
    context->ss = 0x10;

    process->pid = next_pid++;
    process->entry = entry;
    process->context = context;
    process->type = PROCESS_KERNEL;
    process->stack_base = stack_base;
    process->stack_size = PROCESS_STACK_SIZE;
    process->kernel_stack_base = NULL;
    process->kernel_stack_size = 0;
    process->state = PROCESS_READY;
    process->block_reason = REASON_NONE;
    process->wake_tick = 0;
    process->next = NULL;

    return process;
}

Process* process_create_user(void (*entry)(void)) {
    if (entry == NULL) {
        return NULL;
    }

    Process* process = k_malloc(sizeof(Process));
    if (process == NULL) {
        return NULL;
    }

    void* kernel_stack_base = k_malloc(KERNEL_STACK_SIZE);
    if (kernel_stack_base == NULL) {
        k_free(process);
        return NULL;
    }

    u64 user_stack_top = USER_STACK_TOP;
    u64 user_stack_base = user_stack_top - USER_STACK_SIZE;
    if (!process_map_user_stack(user_stack_base)) {
        k_free(kernel_stack_base);
        k_free(process);
        return NULL;
    }

    if (!process_map_user_code(USER_CODE_BASE)) {
        k_free(kernel_stack_base);
        k_free(process);
        return NULL;
    }

    u64 kernel_stack_top = (u64)kernel_stack_base + KERNEL_STACK_SIZE;
    kernel_stack_top = align_down(kernel_stack_top, 16);

    CPUContext* context = (CPUContext*)(kernel_stack_top - sizeof(CPUContext));
    mem_set(context, 0, sizeof(CPUContext));

    context->rip = USER_CODE_BASE;
    context->cs = 0x2B;
    context->rflags = 0x202;
    context->rsp = user_stack_top;
    context->ss = 0x23;

    process->pid = next_pid++;
    process->entry = entry;
    process->context = context;
    process->type = PROCESS_USER;

    process->stack_base = (void*)user_stack_base;
    process->stack_size = USER_STACK_SIZE;

    process->kernel_stack_base = kernel_stack_base;
    process->kernel_stack_size = KERNEL_STACK_SIZE;

    process->state = PROCESS_READY;
    process->block_reason = REASON_NONE;
    process->wake_tick = 0;
    process->next = NULL;
    
    return process;
}

void process_exit(void) {
    Process* process = scheduler_get_current();
    if (process == NULL) {
        panic("process_exit called without current process");
    }

    process->state = PROCESS_TERMINATED;

    scheduler_yield();
    panic("process_exit returned unexpectedly");
}

void process_block(void) {
    Process* process = scheduler_get_current();
    if (process == NULL) {
        panic("process_block called without current process");
    }

    process->state = PROCESS_BLOCKED;
    scheduler_yield();
}

void process_wake(Process* process) {
    if (process == NULL) {
        return;
    }   

    if (process->state == PROCESS_BLOCKED) {
        process->state = PROCESS_READY;
    }
}

void process_sleep(u64 milliseconds) {
    Process* process = scheduler_get_current();
    if (process == NULL) {
        panic("process_sleep called without current process");
    }

    if (milliseconds == 0) {
        scheduler_yield();
        return;
    }

    u64 ticks = timer_ticks_from_ms(milliseconds);
    if (ticks == 0) {
        panic("timer not initialized");
    }

    process->wake_tick = timer_get_ticks() + ticks;
    process->block_reason = REASON_SLEEP;
    process->state = PROCESS_BLOCKED;

    scheduler_yield();
}

void process_trampoline(void) {
    Process* process = scheduler_get_current();
    if (process == NULL || process->entry == NULL) {
        process_exit();
    }

    process->entry();
    process_exit();
}

static bool process_map_user_stack(u64 user_stack_base) {
    for (u64 offset = 0; offset < USER_STACK_SIZE; offset += PAGE_SIZE) {
        u64 physical_page = pmm_alloc_page();
        if (physical_page == 0) {
            return false;
        }

        u64 mapped_address;
        if (!physical_to_virtual(physical_page, &mapped_address)) {
            pmm_free_page(physical_page);
            return false;
        }

        mem_set((void*)mapped_address, 0, PAGE_SIZE);

        u64 virtual_address = user_stack_base + offset;
        if (!paging_map_page(virtual_address, physical_page, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER)) {
            return false;
        }
    }

    return true;
}

static bool process_map_user_code(u64 user_code_address) {
    u64 physical_page = pmm_alloc_page();
    if (physical_page == 0) {
        return false;
    }

    if (!paging_map_page(user_code_address, physical_page, PAGE_PRESENT | PAGE_USER)) {
        pmm_free_page(physical_page);
        return false;
    }

    u64 mapped_address;
    if (!physical_to_virtual(physical_page, &mapped_address)) {
        paging_unmap_page(user_code_address);
        pmm_free_page(physical_page);
        return false;
    }

    mem_set((void*)mapped_address, 0, PAGE_SIZE);

    u8* code = (u8*)mapped_address;
    code[0] = 0xEB;
    code[1] = 0xFE;

    return true;
}