#ifndef TERMINAL_H
#define TERMINAL_H

#include <types/types.h>

/**
 * Maximum number of columns supported by the terminal.
 */
#define TERMINAL_MAX_WIDTH   80

/**
 * Maximum number of rows supported by the terminal.
 */
#define TERMINAL_MAX_HEIGHT  25

/**
 * Represents a VGA text-mode terminal.
 */
typedef struct {
    /**
     * Address of the VGA text buffer;
     */
    volatile char* video;

    /**
     * Current cursor column.
     */
    u8 cursor_x;

    /**
     * Current cursor row.
     */
    u8 cursor_y;
} Terminal;

/**
 * Creates and initializes a terminal.
 * 
 * @return Initialized terminal.
 */
Terminal create_terminal();

/**
 * Writes a single character to the terminal.
 * 
 * Handles newline characters and automatically scrolls
 * when the cursor reaches the bottom of the screen.
 * 
 * @param term Terminal instance.
 * @param c Character to write.
 */
void term_putc(Terminal* term, char c);

/**
 * Writes a null-terminated string to the terminal.
 * 
 * @param term Terminal instance.
 * @param str String to write.
 */
void term_write(Terminal* term, const char* str);

/**
 * Writes a signed integer in decimal notation.
 * 
 * @param term Terminal instance.
 * @param value Integer to write.
 */
void term_write_int(Terminal* term, int value);

/**
 * Writes an unsigned 64-bit integer in hexadecimal notation.
 * 
 * The hexadecimal prefix "0x" is not added automatically.
 * 
 * @param term Terminal instance.
 * @param value Value to write.
 */
void term_write_hex(Terminal* term, u64 value);

#endif