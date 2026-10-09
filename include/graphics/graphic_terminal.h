#ifndef GRAPHIC_TERMINAL_H
#define GRAPHIC_TERMINAL_H

#include <graphics/color.h>
#include <graphics/font.h>
#include <math/vector.h>
#include <types/types.h>

/**
 * A text terminal drawn with a bitmap font inside a rectangle of the
 * renderer.
 *
 * Fill it with graphic_terminal_init(). All drawing goes to the
 * renderer backbuffer, so call renderer_present() (or
 * renderer_present_rect()) to show it on screen.
 */
typedef struct {
    /**
     * Top-left corner of the terminal area, in renderer coordinates.
     */
    Vector2u origin;

    /**
     * Size of the terminal area, in pixels (x = width, y = height).
     */
    Size2u size;

    /**
     * Position of the next character, in pixels, relative to origin.
     */
    Vector2u cursor;

    /**
     * Font used to draw the text.
     */
    const BitmapFont* font;

    /**
     * Integer scale applied to the font (1 = native size).
     */
    u8 scale;

    u8 line_spacing;

    /**
     * Color of the text.
     */
    Color foreground;

    /**
     * Color used to erase: clear, backspace and the line freed by
     * scrolling.
     */
    Color background;
} GraphicTerminal;

/**
 * Settings for graphic_terminal_init(): the same fields as the
 * terminal, without the cursor, which always starts at (0, 0).
 */
typedef struct {
    /**
     * Top-left corner of the terminal area, in renderer coordinates.
     */
    Vector2u origin;

    /**
     * Size of the terminal area, in pixels (x = width, y = height).
     */
    Size2u size;

    /**
     * Font used to draw the text. Must not be NULL.
     */
    const BitmapFont* font;

    /**
     * Integer scale applied to the font. 0 is treated as 1.
     */
    u8 scale;

    u8 line_spacing;

    /**
     * Color of the text.
     */
    Color foreground;

    /**
     * Color used to erase.
     */
    Color background;
} GraphicTerminalConfig;

/**
 * Initializes a terminal from a configuration.
 *
 * Copies the settings and puts the cursor at (0, 0). Nothing is drawn:
 * call graphic_terminal_clear() to paint the background. Does nothing
 * if terminal, config or config->font is NULL.
 *
 * @param terminal Terminal to initialize.
 * @param config Settings to copy from.
 */
void graphic_terminal_init(GraphicTerminal* terminal, const GraphicTerminalConfig* config);

/**
 * Writes one character at the cursor and advances it.
 *
 * A '\n' moves the cursor to the start of the next line. If a
 * character does not fit in the rest of the line, the cursor wraps to
 * the next line first. When the next line would not fit in the
 * terminal height, the contents scroll up by one line and the cursor
 * stays on the last line. Other control characters have no special
 * handling and are drawn as ordinary glyphs.
 *
 * Only the glyph pixels are drawn, in the foreground color: the cell
 * background is not painted, so a character drawn over existing
 * content leaves the old pixels showing.
 *
 * @param terminal Terminal to write to. Ignored if NULL or without a
 *                 font.
 * @param character Character to write.
 */
void graphic_terminal_putchar(GraphicTerminal* terminal, char character);

/**
 * Writes a string, character by character, with
 * graphic_terminal_putchar().
 *
 * @param terminal Terminal to write to.
 * @param text NUL-terminated string. Ignored if NULL.
 */
void graphic_terminal_write(GraphicTerminal* terminal, const char* text);

/**
 * Erases the character before the cursor.
 *
 * Moves the cursor back one cell and paints that cell with the
 * background color. It does nothing at the start of a line: it does
 * not go back to the previous line.
 *
 * @param terminal Terminal to erase on. Ignored if NULL or without a
 *                 font.
 */
void graphic_terminal_backspace(GraphicTerminal* terminal);

/**
 * Clears the terminal.
 *
 * Fills the whole terminal area with the background color and puts
 * the cursor back at (0, 0). Nothing outside the area is touched.
 *
 * @param terminal Terminal to clear. Ignored if NULL.
 */
void graphic_terminal_clear(GraphicTerminal* terminal);

#endif