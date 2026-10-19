#include <scheduler/scheduler.h>
#include <process/process.h>
#include <memory/heap.h>
#include <cpu/tss.h>

#include <panic/panic.h>
#include <timer/timer.h>
#include <types/types.h>

static Process bootstrap_process;
static Process* idle_process;

static Process* first_process;
static Process* last_process;
static Process* current_process;

static void idle_task(void);
static Process* scheduler_find_next_ready(void);

extern void scheduler_yield_asm(void);

void scheduler_init(void) {
    bootstrap_process.pid = 0;
    bootstrap_process.state = PROCESS_RUNNING;
    bootstrap_process.context = NULL;
    bootstrap_process.type = PROCESS_KERNEL;
    bootstrap_process.stack_base = NULL;
    bootstrap_process.stack_size = 0;
    bootstrap_process.kernel_stack_base = NULL;
    bootstrap_process.kernel_stack_size = 0;
    bootstrap_process.next = NULL;
    bootstrap_process.block_reason = REASON_NONE;
    bootstrap_process.wake_tick = 0;

    first_process = &bootstrap_process;
    last_process = &bootstrap_process;
    current_process = &bootstrap_process;

    idle_process = process_create(idle_task);
    if (idle_process == NULL) {
        panic("Failed to create idle process");
    }
}

void scheduler_tick(void) {
    u64 now = timer_get_ticks();

    Process* process = first_process;
    while (process != NULL) {
        if (
            process->state == PROCESS_BLOCKED &&
            process->block_reason == REASON_SLEEP &&
            now >= process->wake_tick
        ) {
            process->state = PROCESS_READY;
            process->block_reason = REASON_NONE;
            process->wake_tick = 0;
        }

        process = process->next;
    }
}

void scheduler_yield(void) {
    scheduler_yield_asm();
}

void scheduler_add_process(Process* process) {
    if (process == NULL) {
        return;
    }

    process->next = NULL;
    if (first_process == NULL) {
        first_process = process;
        last_process = process;
        return;
    }

    last_process->next = process;
    last_process = process;
}

void scheduler_reap_terminated(void) {
    Process* previous = NULL;
    Process* process = first_process;

    while (process != NULL) {
        Process* next = process->next;
        if (process == &bootstrap_process) {
            previous = process;
            process = next;
            continue;
        }

        if (process == idle_process) {
            previous = process;
            process = next;
            continue;
        }

        if (process == current_process) {
            previous = process;
            process = next;
            continue;
        }

        if (process->state == PROCESS_TERMINATED) {
            if (previous != NULL) {
                previous->next = next;
            } else {
                first_process = next;
            }

            if (process == last_process) {
                last_process = previous;
            }

            k_free(process->stack_base);
            k_free(process);

            process = next;
            continue;
        }

        previous = process;
        process = next;
    }
}

CPUContext* scheduler_schedule(CPUContext* context) {
    if (context == NULL) {
        return NULL;
    }

    if (current_process != NULL) {
        current_process->context = context;

        if (current_process->state == PROCESS_RUNNING) {
            current_process->state = PROCESS_READY;
        }
    }

    Process* next = scheduler_find_next_ready();
    if (next == NULL) {
        if (current_process != NULL) {
            current_process->state = PROCESS_RUNNING;
            return current_process->context;
        }

        return context;
    }

    if (next->context == NULL) {
        if (current_process != NULL) {
            current_process->state = PROCESS_RUNNING;
        }

        return context;
    }

    if (next->type == PROCESS_USER) {
        if (next->kernel_stack_base == NULL || next->kernel_stack_size == 0) {
            panic("user process without kernel stack");
        }

        u64 kernel_stack_top = (u64)next->kernel_stack_base + next->kernel_stack_size;
        tss_set_rsp0(kernel_stack_top);
    }

    next->state = PROCESS_RUNNING;
    current_process = next;

    return next->context;
}

Process* scheduler_get_current(void) {
    return current_process;
}

static void idle_task(void) {
    while (1) {
        asm volatile ("hlt");
    }
}

static Process* scheduler_find_next_ready(void) {
    if (first_process == NULL) {
        return NULL;
    }

    Process* candidate;
    if (current_process == NULL) {
        candidate = first_process;
    } else {
        candidate = current_process->next;
        if (candidate == NULL) {
            candidate = first_process;
        }
    }

    Process* start = candidate;
    do {
        if (candidate->state == PROCESS_READY) {
            return candidate;
        }

        candidate = candidate->next;
        if (candidate == NULL) {
            candidate = first_process;
        }
    } while (candidate != start);

    if (idle_process != NULL && idle_process->state == PROCESS_READY) {
        return idle_process;
    }

    return NULL;
}