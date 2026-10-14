#include <scheduler/scheduler.h>
#include <types/types.h>

static Process bootstrap_process;

static Process* first_process;
static Process* last_process;
static Process* current_process;

static Process* scheduler_find_next_ready(void);

void scheduler_init(void) {
    bootstrap_process.pid = 0;
    bootstrap_process.state = PROCESS_RUNNING;
    bootstrap_process.context = NULL;
    bootstrap_process.stack_base = NULL;
    bootstrap_process.stack_size = 0;
    bootstrap_process.next = NULL;

    first_process = &bootstrap_process;
    last_process = &bootstrap_process;
    current_process = &bootstrap_process;
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

    next->state = PROCESS_RUNNING;
    current_process = next;

    return next->context;
}

Process* scheduler_get_current(void) {
    return current_process;
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

    return NULL;
}