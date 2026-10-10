#include <keyboard/keymap.h>

static const char numbers[] = {
    '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9'
};

static const char shifted_numbers[] = {
    ')', '!', '@', '#', '$',
    '%', '\0', '&', '*', '('
};

bool keymap_translate(KeyEvent event, char* character) {
    if (character == NULL) {
        return false;
    }

    if (event.state != KEY_PRESSED) {
        return false;
    }

    bool shifted = event.keyboard_state.lshift || event.keyboard_state.rshift;
    if (event.key >= KEY_A && event.key <= KEY_Z) {
        bool uppercase = shifted ^ event.keyboard_state.caps_lock;

        u8 offset = (u8)(event.key - KEY_A);
        *character = (uppercase ? 'A' : 'a') + offset;
        return true;
    }

    if (event.key >= KEY_0 && event.key <= KEY_9) {
        u32 index = event.key - KEY_0;
        *character = shifted
                     ? shifted_numbers[index]
                     : numbers[index];

        if (*character == '\0') {
            return false;
        }

        return true;
    }

    if (event.key >= KEY_NUM_0 && event.key <= KEY_NUM_9) {
        u32 index = event.key - KEY_NUM_0;
        *character = '0' + index;

        return true;
    }
    
    switch (event.key) {
        case KEY_SPACE:
            *character = ' ';
            return true;
        case KEY_TAB:
            // TODO
            return false;
        case KEY_APOSTROPHE:
            *character = shifted ? '"' : '\'';
            return true;
        case KEY_MINUS:
            *character = shifted ? '_' : '-';
            return true;
        case KEY_EQUALS:
            *character = shifted ? '+' : '=';
            return true;
        case KEY_LEFT_BRACKET:
            *character = shifted ? '{' : '[';
            return true;
        case KEY_RIGHT_BRACKET:
            *character = shifted ? '}' : ']';
            return true;
        case KEY_TILDE:
            *character = shifted ? '^' : '~';
            return true;
        case KEY_BACKSLASH:
            *character = shifted ? '|' : '\\';
            return true;
        case KEY_COMMA:
            *character = shifted ? '<' : ',';
            return true;
        case KEY_PERIOD:
            *character = shifted ? '>' : '.';
            return true;
        case KEY_SEMICOLON:
            *character = shifted ? ':' : ';';
            return true;
        case KEY_SLASH:
            *character = shifted ? '?' : '/';
            return true;
        case KEY_ENTER:
        case KEY_NUM_ENTER:
            *character = '\n';
            return true;
        case KEY_NUM_DIVIDE:
            *character = '/';
            return true;
        case KEY_NUM_MULTIPLY:
            *character = '*';
            return true;
        case KEY_NUM_SUBTRACT:
            *character = '-';
            return true;
        case KEY_NUM_ADD:
            *character = '+';
            return true;
        case KEY_NUM_DECIMAL:
            *character = '.';
            return true;
        default:
            return false;
    }
}