#ifndef PANIC_CONSOLE_H
#define PANIC_CONSOLE_H

#include <types/types.h>

/**
 * Clears the panic console and resets the cursor to its starting
 * position.
 *
 * Draws directly to the framebuffer, independent of the normal
 * terminal, so it works even when the terminal is in an unknown or
 * corrupted state. Meant to be called once, at the start of a panic.
 */
void panic_console_clear(void);

/**
 * Writes a string to the panic console.
 *
 * Renders each character with the built-in Terminus font and
 * advances the cursor; '\n' moves to the next line. Characters
 * without a glyph in the font are skipped.
 *
 * @param text Null-terminated string to write. Does nothing if NULL.
 */
void panic_console_write(const char* text);

/**
 * Writes a labeled 64-bit value to the panic console, in hexadecimal,
 * followed by a newline.
 *
 * Format: "label: 0xVALUE\n" (all 16 hex digits, zero-padded).
 *
 * @param label Null-terminated label printed before the value.
 *              Skipped if NULL.
 * @param value Value to print in hexadecimal.
 */
void panic_console_write_hex(const char* label, u64 value);

#endif