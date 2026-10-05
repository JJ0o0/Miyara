#ifndef EVENT_H
#define EVENT_H

#include <keyboard/keyboard.h>
#include <mouse/mouse.h>

#include <types/types.h>

/**
 * Identifies which member of EventData an Event carries.
 */
typedef enum {
    /**
     * A key press or release; the payload is EventData.keyboard.
     */
    EVENT_KEYBOARD,

    /**
     * A mouse packet (movement and button state); the payload is
     * EventData.mouse.
     */
    EVENT_MOUSE
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

    /**
     * Valid when type == EVENT_MOUSE.
     */
    MouseEvent mouse;
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
 * Not synchronized: it must only be called with interrupts disabled,
 * which is the case inside an IRQ handler (the IDT gates disable
 * interrupts on entry, so handlers do not interrupt each other). It
 * is therefore safe for the keyboard and mouse handlers to share the
 * queue, but not to call this from regular code with interrupts
 * enabled.
 *
 * @param event Event to enqueue (copied by value).
 */
void add_event(Event event);

/**
 * Pops the oldest event from the event queue, if any.
 *
 * Events are returned in the order they were added, regardless of
 * type. Safe to call from regular code: it briefly disables
 * interrupts while it touches the queue and restores their previous
 * state afterwards.
 *
 * @param event Output parameter; filled with the dequeued event on
 * success. Left untouched if the queue is empty.
 * @return true if an event was dequeued, false if the queue was empty.
 */
bool get_event(Event* event);

#endif