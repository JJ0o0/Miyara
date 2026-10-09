#include <keyboard/keyboard.h>

#include <types/types.h>

#include <event/event.h>
#include <io/ps2.h>

#define KEYBOARD_CMD_ENABLE_SCANNING 0xF4
#define KEYBOARD_RESPONSE_ACK        0xFA

static const KeyCode keymap[256] = {
    [0x1E] = KEY_A,
    [0x30] = KEY_B,
    [0x2E] = KEY_C,
    [0x20] = KEY_D,
    [0x12] = KEY_E,
    [0x21] = KEY_F,
    [0x22] = KEY_G,
    [0x23] = KEY_H,
    [0x17] = KEY_I,
    [0x24] = KEY_J,
    [0x25] = KEY_K,
    [0x26] = KEY_L,
    [0x32] = KEY_M,
    [0x31] = KEY_N,
    [0x18] = KEY_O,
    [0x19] = KEY_P,
    [0x10] = KEY_Q,
    [0x13] = KEY_R,
    [0x1F] = KEY_S,
    [0x14] = KEY_T,
    [0x16] = KEY_U,
    [0x2F] = KEY_V,
    [0x11] = KEY_W,
    [0x2D] = KEY_X,
    [0x15] = KEY_Y,
    [0x2C] = KEY_Z,

    [0x0B] = KEY_0,
    [0x02] = KEY_1,
    [0x03] = KEY_2,
    [0x04] = KEY_3,
    [0x05] = KEY_4,
    [0x06] = KEY_5,
    [0x07] = KEY_6,
    [0x08] = KEY_7,
    [0x09] = KEY_8,
    [0x0A] = KEY_9,

    [0x29] = KEY_APOSTROPHE,
    [0x0C] = KEY_MINUS,
    [0x0D] = KEY_EQUALS,
    [0x1A] = KEY_ACUTE,
    [0x1B] = KEY_LEFT_BRACKET,
    [0x27] = KEY_C_CEDILLA,
    [0x28] = KEY_TILDE,
    [0x2B] = KEY_RIGHT_BRACKET,
    [0x56] = KEY_BACKSLASH,
    [0x33] = KEY_COMMA,
    [0x34] = KEY_PERIOD,
    [0x35] = KEY_SEMICOLON,
    [0x73] = KEY_SLASH,

    [0x52] = KEY_NUM_0,
    [0x4f] = KEY_NUM_1,
    [0x50] = KEY_NUM_2,
    [0x51] = KEY_NUM_3,
    [0x4B] = KEY_NUM_4,
    [0x4C] = KEY_NUM_5,
    [0x4D] = KEY_NUM_6,
    [0x47] = KEY_NUM_7,
    [0x48] = KEY_NUM_8,
    [0x49] = KEY_NUM_9,

    [0x37] = KEY_NUM_MULTIPLY,
    [0x4A] = KEY_NUM_SUBTRACT,
    [0x4E] = KEY_NUM_ADD,
    [0x53] = KEY_NUM_DECIMAL,

    [0x2A] = KEY_LSHIFT,
    [0x36] = KEY_RSHIFT,
    [0x1D] = KEY_LCTRL,
    [0x38] = KEY_LALT,

    [0x3A] = KEY_CAPS_LOCK,
    [0x45] = KEY_NUM_LOCK,
    [0x46] = KEY_SCROLL_LOCK,

    [0x0F] = KEY_TAB,
    [0x0E] = KEY_BACKSPACE,
    [0x01] = KEY_ESC,
    [0x39] = KEY_SPACE,
    [0x1C] = KEY_ENTER,

    [0x3B] = KEY_F1,
    [0x3C] = KEY_F2,
    [0x3D] = KEY_F3,
    [0x3E] = KEY_F4,
    [0x3F] = KEY_F5,
    [0x40] = KEY_F6,
    [0x41] = KEY_F7,
    [0x42] = KEY_F8,
    [0x43] = KEY_F9,
    [0x44] = KEY_F10,
    [0x57] = KEY_F11,
    [0x58] = KEY_F12,
};

static const KeyCode extended_keymap[256] = {
    [0x4B] = KEY_LEFT,
    [0x4D] = KEY_RIGHT,
    [0x48] = KEY_UP,
    [0x50] = KEY_DOWN,

    [0x35] = KEY_NUM_DIVIDE,
    [0x1C] = KEY_NUM_ENTER,

    [0x1D] = KEY_RCTRL,
    [0x38] = KEY_RALT,

    [0x5B] = KEY_LSUPER,
    [0x5C] = KEY_RSUPER,

    [0x52] = KEY_INSERT,
    [0x53] = KEY_DELETE,
    [0x47] = KEY_HOME,
    [0x4F] = KEY_END,
    [0x49] = KEY_PAGE_UP,
    [0x51] = KEY_PAGE_DOWN,
};

typedef enum {
    SCANCODE_NORMAL,
    SCANCODE_EXTENDED,
    SCANCODE_E1,
    SCANCODE_PRINT_SCREEN,
} ScancodeState;

typedef enum {
    PRINT_SCREEN_PRESS,
    PRINT_SCREEN_RELEASE
} PrintScreenState;

static ScancodeState scancode_state = SCANCODE_NORMAL;
static u8 e1_bytes = 0;
static PrintScreenState print_screen_state;

static KeyboardState keyboard_state = {0};

static void keyboard_state_update(KeyboardState* state, KeyEvent event);
static bool keyboard_parse_scancode(u8 scancode, KeyEvent* event);
static bool keyboard_parse_e1(u8 scancode, KeyEvent* event);
static bool keyboard_parse_print_screen(u8 scancode, KeyEvent* event);

bool keyboard_init(void) {
    if (!ps2_write_first_port(KEYBOARD_CMD_ENABLE_SCANNING)) {
        return false;
    }

    u8 response;
    if (!ps2_read_data(&response)) {
        return false;
    }

    return response == KEYBOARD_RESPONSE_ACK;
}

void keyboard_handle(void) {
    u8 scancode = ps2_read_data_now();

    KeyEvent event;
    if (!keyboard_parse_scancode(scancode, &event)) {
        return;
    }

    keyboard_state_update(&keyboard_state, event);
    event.keyboard_state = keyboard_state;

    Event system_event = {
        .type = EVENT_KEYBOARD,
        .data.keyboard = event
    };

    add_event(&system_event);
}

static void keyboard_state_update(KeyboardState* state, KeyEvent event) {
    switch (event.key) {
        case KEY_LSHIFT:
            state->lshift = event.state == KEY_PRESSED;
            break;
        case KEY_RSHIFT:
            state->rshift = event.state == KEY_PRESSED;
            break;
        case KEY_LCTRL:
            state->lctrl = event.state == KEY_PRESSED;
            break;
        case KEY_RCTRL:
            state->rctrl = event.state == KEY_PRESSED;
            break;
        case KEY_LALT:
            state->lalt = event.state == KEY_PRESSED;
            break;
        case KEY_RALT:
            state->ralt = event.state == KEY_PRESSED;
            break;
        case KEY_LSUPER:
            state->lsuper = event.state == KEY_PRESSED;
            break;
        case KEY_RSUPER:
            state->rsuper = event.state == KEY_PRESSED;
            break;
        case KEY_CAPS_LOCK:
            if (event.state != KEY_PRESSED) { 
                break;
            }

            state->caps_lock = !state->caps_lock;
            break;
        case KEY_NUM_LOCK:
            if (event.state != KEY_PRESSED) { 
                break;
            }

            state->num_lock = !state->num_lock;
            break; 
        case KEY_SCROLL_LOCK:
            if (event.state != KEY_PRESSED) { 
                break;
            }

            state->scroll_lock = !state->scroll_lock;
            break;
        default:
            break;
    }
}

static bool keyboard_parse_scancode(u8 scancode, KeyEvent* event) {
    if (scancode_state == SCANCODE_E1) {
        return keyboard_parse_e1(scancode, event);
    }

    if (scancode_state == SCANCODE_PRINT_SCREEN) {
        return keyboard_parse_print_screen(scancode, event);
    }

    if (scancode == 0xE0) {
        scancode_state = SCANCODE_EXTENDED;
        return false;
    }

    if (scancode == 0xE1) {
        scancode_state = SCANCODE_E1;
        e1_bytes = 0;
        return false;
    }

    if (scancode_state == SCANCODE_EXTENDED) {
        if (scancode == 0x2A) {
            print_screen_state = PRINT_SCREEN_PRESS;
            scancode_state = SCANCODE_PRINT_SCREEN;
            return false;
        }

        if (scancode == 0xB7) {
            print_screen_state = PRINT_SCREEN_RELEASE;
            scancode_state = SCANCODE_PRINT_SCREEN;
            return false;
        }
    }

    u8 code = scancode & 0x7F;
    event->key = scancode_state == SCANCODE_EXTENDED 
                                ? extended_keymap[code] 
                                : keymap[code];
    
    scancode_state = SCANCODE_NORMAL;

    if (event->key == KEY_UNKNOWN) {
        return false;
    }

    event->state = scancode & 0x80 ? KEY_RELEASED : KEY_PRESSED;
    return true;
}

static bool keyboard_parse_e1(u8 scancode, KeyEvent* event) {
    const u8 expected[] = { 0x1D, 0x45, 0xE1, 0x9D, 0xC5 };
    if (scancode != expected[e1_bytes]) {
        scancode_state = SCANCODE_NORMAL;
        e1_bytes = 0;
        return false;
    }
    
    e1_bytes++;

    if (e1_bytes < sizeof(expected)) {
        return false;
    }

    event->key = KEY_PAUSE;
    event->state = KEY_PRESSED;

    scancode_state = SCANCODE_NORMAL;
    e1_bytes = 0;
    return true;
}

static bool keyboard_parse_print_screen(u8 scancode, KeyEvent* event) {
    if (scancode == 0xE0) {
        return false;
    }

    if (print_screen_state == PRINT_SCREEN_PRESS && scancode == 0x37) {
        event->key = KEY_PRINT_SCREEN;
        event->state = KEY_PRESSED;
    } else if (print_screen_state == PRINT_SCREEN_RELEASE && scancode == 0xAA) {
        event->key = KEY_PRINT_SCREEN;
        event->state = KEY_RELEASED;
    } else {
        scancode_state = SCANCODE_NORMAL;
        return false;
    }

    scancode_state = SCANCODE_NORMAL;
    return true;
}