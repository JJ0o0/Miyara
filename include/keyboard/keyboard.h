#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <types/types.h>

/**
 * Logical key identifiers, independent of scancode set or layout.
 *
 * Produced by translating raw PS/2 scancodes (see keyboard.c) into
 * a single, layout-agnostic set of keys.
 */
typedef enum {
    /**
     * Scancode had no mapping (unmapped key, or a byte swallowed
     * while parsing a multi-byte sequence).
     */
    KEY_UNKNOWN = 0,

    // Letters
    KEY_A, KEY_B, KEY_C,
    KEY_D, KEY_E, KEY_F,
    KEY_G, KEY_H, KEY_I,
    KEY_J, KEY_K, KEY_L,
    KEY_M, KEY_N, KEY_O,
    KEY_P, KEY_Q, KEY_R,
    KEY_S, KEY_T, KEY_U,
    KEY_V, KEY_W, KEY_X,
    KEY_Y, KEY_Z,

    // Numbers (top row)
    KEY_0,
    KEY_1, KEY_2, KEY_3,
    KEY_4, KEY_5, KEY_6,
    KEY_7, KEY_8, KEY_9,

    // Numpad
    KEY_NUM_0,
    KEY_NUM_1,
    KEY_NUM_2,
    KEY_NUM_3,
    KEY_NUM_4,
    KEY_NUM_5,
    KEY_NUM_6,
    KEY_NUM_7,
    KEY_NUM_8,
    KEY_NUM_9,

    KEY_NUM_DIVIDE,
    KEY_NUM_MULTIPLY,
    KEY_NUM_SUBTRACT,
    KEY_NUM_ADD,
    KEY_NUM_DECIMAL,

    /**
     * Numpad Enter. Distinct from KEY_ENTER: it arrives as an
     * extended (0xE0-prefixed) scancode.
     */
    KEY_NUM_ENTER,

    // Arrows (extended scancodes)
    KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,

    // Modifiers
    KEY_LSHIFT, KEY_RSHIFT,

    /**
     * Left Ctrl. Shares scancode 0x1D with KEY_RCTRL; distinguished
     * by whether the scancode was extended (0xE0-prefixed).
     */
    KEY_LCTRL,

    /**
     * Right Ctrl. Extended scancode; see KEY_LCTRL.
     */
    KEY_RCTRL,

    /**
     * Left Alt. Shares scancode 0x38 with KEY_RALT; distinguished
     * by whether the scancode was extended (0xE0-prefixed).
     */
    KEY_LALT,
    
    /**
     * Right Alt. Extended scancode; see KEY_LALT.
     */
    KEY_RALT,

    // Locks (toggled on press; state tracked in KeyboardState)
    KEY_CAPS_LOCK,
    KEY_NUM_LOCK,
    KEY_SCROLL_LOCK,

    // Functions
    KEY_TAB,
    KEY_BACKSPACE,
    KEY_ESC,
    KEY_SPACE,
    KEY_ENTER,

    // Extended scancodes
    KEY_LSUPER, KEY_RSUPER,

    KEY_F1, KEY_F2, KEY_F3,
    KEY_F4, KEY_F5, KEY_F6,
    KEY_F7, KEY_F8, KEY_F9,
    KEY_F10, KEY_F11, KEY_F12,

    KEY_INSERT,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,

    /**
     * Print Screen. Sent as a multi-byte sequence on both press
     * and release; reassembled internally before being reported as
     * a single KEY_PRESSED/KEY_RELEASED event.
     */
    KEY_PRINT_SCREEN,

    /**
     * Pause/Break key.
     *
     * Sent as a 6-byte E1-prefixed sequence, but the OS only ever sees
     * a press (the key has no distinct release scancode). Reported as
     * a single KEY_PRESSED event once the 5 bytes following the 0xE1
     * prefix have been consumed.
     */
    KEY_PAUSE
} KeyCode;

/**
 * Press/release state of a key event.
 */
typedef enum {
    /**
     * The key went down (make code).
     */
    KEY_PRESSED,

    /**
     * The key was let go (break code).
     */
    KEY_RELEASED
} KeyState;

/**
 * Current state of modifier and lock keys.
 *
 * Updated internally on every key event; reflects "is held" for
 * modifiers and "is active" for locks (toggled on each press).
 */
typedef struct {
    /**
     * True while the corresponding modifier key is held down.
     */
    bool lshift;
    bool rshift;

    bool lctrl;
    bool rctrl;

    bool lalt;
    bool ralt;

    bool lsuper;
    bool rsuper;

    /**
     * True while the corresponding lock is active. Toggled on each
     * press of its key (releases are ignored).
     */
    bool caps_lock;
    bool num_lock;
    bool scroll_lock;
} KeyboardState;

/**
 * A single key press or release.
 */
typedef struct {
    /**
     * Key this event refers to.
     */
    KeyCode key;

    /**
     * Whether the key was pressed or released.
     */
    KeyState state;

        /**
     * Modifier and lock state right after this event was applied, so
     * the press of a modifier already shows it as active.
     */
    KeyboardState keyboard_state;
} KeyEvent;

/**
 * Enables scanning on the keyboard (first port).
 *
 * Sends the "enable scanning" command (0xF4) to the keyboard and waits
 * for its acknowledgment (0xFA) by polling. Requires ps2_init() to
 * have succeeded. Call it with interrupts disabled, before
 * enable_interrupts(): otherwise the IRQ 1 handler would consume the
 * reply byte.
 *
 * @return true if the keyboard acknowledged the command, false on
 *         timeout or any other response.
 */
bool keyboard_init(void);

/**
 * Reads and processes one scancode from the keyboard controller.
 *
 * Meant to be called from the keyboard IRQ handler (irq_dispatch,
 * IRQ 1). Reads a byte from port 0x60, feeds it through the
 * scancode parser, updates the internal KeyboardState, and, if the
 * byte completed a key event, pushes an EVENT_KEYBOARD event onto
 * the event queue (see event.h).
 *
 * Some scancodes don't produce an event on their own (e.g. the
 * 0xE0/0xE1 prefix bytes, or intermediate bytes of a multi-byte
 * sequence) and are only consumed to advance the parser's internal
 * state.
 */
void keyboard_handle(void);

#endif