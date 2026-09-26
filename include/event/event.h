#ifndef EVENT_H
#define EVENT_H

#include <keyboard/keyboard.h>
#include <types/types.h>

typedef enum {
    EVENT_KEYBOARD
} EventType;

typedef union {
    KeyEvent keyboard;
} EventData;

typedef struct {
    EventType type;
    EventData data;
} Event;

void add_event(Event event);
bool get_event(Event* event);

#endif