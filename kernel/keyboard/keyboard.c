#include <keyboard/keyboard.h>
#include <event/event.h>
#include <log/log.h>
#include <io/io.h>

static const KeyCode keymap[256] = {
    [0x1E] = KEY_A,
    [0x30] = KEY_B,
    [0x2E] = KEY_C,

    [0x39] = KEY_SPACE,
    [0x1C] = KEY_ENTER,
};

void keyboard_handle(void) {
    u8 scancode = io_in8(0x60);
    u8 code = scancode & 0x7F;

    KeyEvent key_event;
    key_event.key = keymap[code];
    key_event.state = scancode & 0x80 ? KEY_RELEASED : KEY_PRESSED;

    Event event;
    event.type = EVENT_KEYBOARD;
    event.data.keyboard = key_event;
    add_event(event);
}
