#ifndef GRAPHIC_TERMINAL_H
#define GRAPHIC_TERMINAL_H

#include <graphics/color.h>
#include <graphics/font.h>
#include <math/vector.h>
#include <math/geometry.h>
#include <types/types.h>

/**
 * Height of the visual text cursor, in pixels.
 *
 * The cursor is rendered as a horizontal underline at the bottom of
 * the current character cell.
 */
#define GRAPHIC_TERMINAL_CURSOR_HEIGHT      2

/**
 * Maximum supported cursor width, in pixels.
 *
 * Used to size the temporary pixel buffer that stores the contents
 * underneath the cursor while it is visible.
 */
#define GRAPHIC_TERMINAL_CURSOR_MAX_WIDTH   32

/**
 * Cursor blink interval, in milliseconds.
 */
#define GRAPHIC_TERMINAL_CURSOR_BLINK_MS    500

/**
 * Number of character columns between tab stops.
 */
#define GRAPHIC_TERMINAL_TAB_WIDTH 4

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
     * Position of the next character, in pixels, relative to the terminal
     * content area (after padding).
     */
    Vector2u cursor;

    /**
     * Whether the visual cursor is currently drawn.
     */
    bool cursor_visible;

    /**
     * Saved renderer pixels underneath the visual cursor.
     *
     * The pixels are captured before the cursor is drawn and restored
     * when the cursor is hidden, preventing the cursor from damaging
     * glyph pixels underneath it.
     */
    u32 cursor_saved_pixels[GRAPHIC_TERMINAL_CURSOR_MAX_WIDTH * GRAPHIC_TERMINAL_CURSOR_HEIGHT];

    /**
     * Timer tick at which the cursor blink state was last toggled.
     */
    u64 cursor_last_blink_tick;

    /**
     * Font used to draw the text.
     */
    const BitmapFont* font;

    /**
     * Integer scale applied to the font (1 = native size).
     */
    u8 scale;

    /**
     * Extra vertical spacing, in pixels, added between terminal lines.
     */
    u8 line_spacing;

    /**
     * Internal spacing between the terminal bounds and its content area.
     *
     * Text, the text cursor and scrolling operate inside the remaining
     * content area.
     */
    Insets padding;

    /**
     * Total vertical distance, in pixels, that the terminal content has
     * scrolled since initialization.
     *
     * This value only increases and can be used by higher-level components
     * to keep logical positions synchronized with visible terminal content.
     */
    u64 scroll_offset;

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

    /**
     * Extra vertical spacing, in pixels, added between terminal lines.
     */
    u8 line_spacing;

    /**
     * Internal spacing between the terminal bounds and its content area.
     *
     * Text, the text cursor and scrolling operate inside the remaining
     * content area.
     */
    Insets padding;

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
 * A '\n' moves the cursor to the start of the next line.
 *
 * A '\t' advances to the next tab stop. Tab stops occur every
 * GRAPHIC_TERMINAL_TAB_WIDTH character columns and are implemented
 * as ordinary spaces, so wrapping and scrolling work normally.
 *
 * If a character does not fit in the remaining content width, the
 * cursor wraps to the next line. When the next line would not fit in
 * the content height, the terminal scrolls up by one line.
 *
 * Drawing is restricted to the terminal content area.
 *
 * @param terminal Terminal to write to. Ignored if NULL or without
 *                 a font.
 * @param character Character to write.
 */
void graphic_terminal_putchar(GraphicTerminal* terminal, char character);

/**
 * Draws one character at an explicit position in the terminal content
 * area without changing the terminal cursor.
 *
 * The position is relative to the content area, not the outer terminal
 * origin. Padding is applied automatically.
 *
 * No wrapping or scrolling is performed. If the complete glyph would
 * fall outside the content area, nothing is drawn.
 *
 * @param terminal Terminal to draw on.
 * @param character Character to draw.
 * @param position Position in pixels relative to the content area.
 */
void graphic_terminal_draw_char_at(GraphicTerminal* terminal, char character, Vector2u position);

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

/**
 * Clears a number of character cells starting at the current cursor
 * position.
 *
 * Each cell is painted with the terminal background color. The cursor
 * position itself is not changed.
 *
 * @param terminal Terminal to clear cells on. Ignored if NULL or
 *                 without a font.
 * @param count Number of consecutive character cells to clear.
 */
void graphic_terminal_clear_chars(GraphicTerminal* terminal, u32 count);

/**
 * Draws the visual text cursor at the current terminal cursor position.
 *
 * The pixels underneath the cursor are saved before drawing so they
 * can later be restored by graphic_terminal_hide_cursor().
 *
 * Does nothing if the cursor is already visible.
 *
 * @param terminal Terminal whose cursor should be drawn. Ignored if
 *                 NULL or without a font.
 */
void graphic_terminal_draw_cursor(GraphicTerminal* terminal);

/**
 * Hides the visual text cursor.
 *
 * Restores the pixels that were saved when the cursor was drawn.
 *
 * Does nothing if the cursor is already hidden.
 *
 * @param terminal Terminal whose cursor should be hidden. Ignored if
 *                 NULL or without a font.
 */
void graphic_terminal_hide_cursor(GraphicTerminal* terminal);

/**
 * Toggles the visual cursor between visible and hidden states.
 *
 * @param terminal Terminal whose cursor state should be toggled.
 */
void graphic_terminal_toggle_cursor(GraphicTerminal* terminal);

/**
 * Presents only the visual cursor region from the renderer backbuffer
 * to the framebuffer.
 *
 * This avoids presenting the entire terminal when only the blinking
 * cursor has changed.
 *
 * @param terminal Terminal whose cursor region should be presented.
 */
void graphic_terminal_present_cursor(GraphicTerminal* terminal);

/**
 * Updates the cursor blink state according to the system timer.
 *
 * When GRAPHIC_TERMINAL_CURSOR_BLINK_MS has elapsed since the previous
 * toggle, the cursor visibility is changed and the last blink tick is
 * updated.
 *
 * @param terminal Terminal whose cursor blink state should be updated.
 *
 * @return true if the cursor visual state changed.
 * @return false if no redraw is required.
 */
bool graphic_terminal_update_cursor(GraphicTerminal* terminal);

/**
 * Restarts the visual cursor blink interval from the current timer tick.
 *
 * This is typically called after keyboard input so the cursor remains
 * visible for a full blink interval before toggling again.
 *
 * @param terminal Terminal whose cursor blink timer should be reset.
 */
void graphic_terminal_reset_cursor_blink(GraphicTerminal* terminal);

/**
 * Moves the terminal cursor horizontally to a character column on the
 * current line.
 *
 * The column is relative to the terminal content area. It is converted
 * to pixels using the current glyph width and scale. The vertical cursor
 * position is unchanged.
 *
 * If the requested column is outside the content width, the cursor is
 * clamped to the last valid character cell.
 *
 * @param terminal Terminal whose cursor should be moved.
 * @param column Character column relative to the content area.
 */
void graphic_terminal_set_cursor_column(GraphicTerminal* terminal, u32 column);

/**
 * Scrolls the terminal content area upward by one line.
 *
 * Only the content area is moved; padding is left untouched. The newly
 * exposed bottom line is filled with the background color.
 *
 * The terminal cursor is moved upward by the same amount when possible,
 * and scroll_offset is increased by the line height.
 *
 * @param terminal Terminal to scroll.
 *
 * @return true if scrolling was performed.
 * @return false if the terminal content area cannot contain one line.
 */
bool graphic_terminal_scroll(GraphicTerminal* terminal);

/**
 * Returns the usable content width after subtracting left and right
 * padding. Returns 0 if the padding consumes the entire width.
 */
u32 graphic_terminal_get_content_width(const GraphicTerminal* terminal);

/**
 * Returns the usable content height after subtracting top and bottom
 * padding. Returns 0 if the padding consumes the entire height.
 */
u32 graphic_terminal_get_content_height(const GraphicTerminal* terminal);

/**
 * Returns the width of one scaled font glyph, in pixels.
 */
u32 graphic_terminal_get_char_width(const GraphicTerminal* terminal);

/**
 * Returns the height of one scaled font glyph, in pixels.
 */
u32 graphic_terminal_get_char_height(const GraphicTerminal* terminal);

/**
 * Returns the vertical distance between consecutive terminal lines,
 * equal to the scaled glyph height plus line_spacing.
 */
u32 graphic_terminal_get_line_height(const GraphicTerminal* terminal);

#endif