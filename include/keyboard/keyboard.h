#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum {
    KEY_UNKNOWN = 0,

    KEY_A,
    KEY_B,
    KEY_C,
    KEY_SPACE,

    KEY_ENTER
} KeyCode;

typedef enum {
    KEY_PRESSED,
    KEY_RELEASED
} KeyState;

typedef struct {
    KeyCode key;
    KeyState state;
} KeyEvent;

void keyboard_handle(void);

#endif