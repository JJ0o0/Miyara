#include <event/event.h>

#include <scheduler/scheduler.h> 
#include <process/process.h>

#include <panic/panic.h>
#include <io/io.h>

#define EVENT_QUEUE_SIZE 64
static Event queue[EVENT_QUEUE_SIZE] = {0};

static u32 head = 0;
static u32 tail = 0;
static u32 count = 0;

static Process* event_waiter = NULL;

void add_event(const Event* event) {
    if (event == NULL) {
        return;
    }

    if (count == EVENT_QUEUE_SIZE) {
        return;
    }

    if (head == EVENT_QUEUE_SIZE) { 
        head = 0;
    }

    queue[head] = *event;
    head++;
    count++;

    if (event_waiter != NULL) {
        process_wake(event_waiter);
        event_waiter = NULL;
    }
}

bool get_event(Event* event) {
    if (event == NULL) {
        return false;
    }

    bool was_enabled;
    interrupts_save_and_disable(&was_enabled);

    bool success = false;
    if (count != 0) {
        if (tail == EVENT_QUEUE_SIZE) {
            tail = 0;
        }

        *event = queue[tail];
        tail++;
        count--;

        success = true;
    }

    interrupts_restore(was_enabled);
    return success;
}

void wait_event(Event* event) {
    if (event == NULL) {
        return;
    }

    while (1) {
        bool was_enabled;
        interrupts_save_and_disable(&was_enabled);

        if (count != 0) {
            if (tail == EVENT_QUEUE_SIZE) {
                tail = 0;
            }

            *event = queue[tail];
            tail++;
            count--;

            interrupts_restore(was_enabled);
            return;
        }

        Process* process = scheduler_get_current();
        if (process == NULL) {
            interrupts_restore(was_enabled);
            panic("wait_event called without current process");
        }

        event_waiter = process;

        process->block_reason = REASON_EVENT;
        process->state = PROCESS_BLOCKED;

        scheduler_yield();
        interrupts_restore(was_enabled);
    }
}