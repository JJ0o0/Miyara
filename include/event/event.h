#ifndef EVENT_H
#define EVENT_H

#include <keyboard/keyboard.h>
#include <types/types.h>

/**
 * Identifies which member of EventData an Event carries.
 */
typedef enum {
    /**
     * A key press or release; the payload is EventData.keyboard.
     */
    EVENT_KEYBOARD
} EventType;

/**
 * Payload of an event.
 *
 * Which member is valid is determined by the Event's type field.
 */
typedef union {
    /**
     * Valid when type == EVENT_KEYBOARD.
     */
    KeyEvent keyboard;
} EventData;

/**
 * A single event, as stored in and retrieved from the event queue.
 */
typedef struct {
    /**
     * Kind of event this is; selects the active member of data.
     */
    EventType type;

    /**
     * Event payload.
     */
    EventData data;
} Event;

/**
 * Pushes an event onto the event queue.
 *
 * The queue has a fixed capacity; if it is full, the event is
 * silently dropped.
 *
 * @param event Event to enqueue (copied by value).
 */
void add_event(Event event);

/**
 * Pops the oldest event from the event queue, if any.
 *
 * @param event Output parameter; filled with the dequeued event on
 * success. Left untouched if the queue is empty.
 * @return true if an event was dequeued, false if the queue was empty.
 */
bool get_event(Event* event);

#endif