#include <event/event.h>
#include <io/io.h>

#define EVENT_QUEUE_SIZE 64
static Event queue[EVENT_QUEUE_SIZE] = {0};

static u32 head = 0;
static u32 tail = 0;
static u32 count = 0;

void add_event(Event event) {
    if (count == EVENT_QUEUE_SIZE) {
        return;
    }

    if (head == EVENT_QUEUE_SIZE) { 
        head = 0;
    }

    queue[head] = event;
    head++;
    count++;
}

bool get_event(Event* event) {
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
