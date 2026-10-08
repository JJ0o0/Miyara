#ifndef KEYMAP_H
#define KEYMAP_H

#include <keyboard/keyboard.h>
#include <types/types.h>

/**
 * Translates a key event into the character it types.
 *
 * Handles letters (upper case when Shift XOR Caps Lock), the digit row
 * (Shift gives the US-layout symbols, e.g. Shift+1 is '!'), the keypad
 * digits and operators, space, tab, and both Enter keys (which give
 * '\n'). The keypad digits do not depend on Num Lock or Shift.
 * Punctuation keys have no key code yet, so they are not translated.
 *
 * Only key presses translate: a release always returns false.
 *
 * @param event Key event; its keyboard_state provides Shift and Caps
 *              Lock.
 * @param character Output parameter; receives the character on success
 *                  and is left untouched when this returns false.
 * @return true if the key produces a character; false for a release,
 *         for keys without a character (function keys, arrows,
 *         modifiers, ...), or if character is NULL.
 */
bool keymap_translate(KeyEvent event, char* character);

#endif