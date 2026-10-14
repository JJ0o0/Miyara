#include <process/process.h>

#include <memory/memory.h>
#include <memory/heap.h>

#include <types/types.h>
#include <math/math.h>

static u64 next_pid = 1;

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

    context->rip = (u64)entry;
    context->cs = 0x18;
    context->rflags = 0x202;
    context->rsp = task_rsp;
    context->ss = 0x10;

    process->pid = next_pid++;
    process->context = context;
    process->stack_base = stack_base;
    process->stack_size = PROCESS_STACK_SIZE;
    process->state = PROCESS_READY;
    process->next = NULL;

    return process;
}