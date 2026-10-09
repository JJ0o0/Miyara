#ifndef FONT_H
#define FONT_H

#include <graphics/color.h>
#include <types/types.h>
#include <math/vector.h>

typedef enum {
    FONT_BIT_ORDER_LSB_FIRST,
    FONT_BIT_ORDER_MSB_FIRST
} FontBitOrder;

/**
 * A monochrome bitmap font.
 *
 * Each glyph is stored as glyph_size.y consecutive bytes, one byte per
 * row, so glyphs can be at most 8 pixels wide. The bit order within each
 * row is defined by bit_order. Glyphs are indexed by character code.
 */
typedef struct {
    /**
     * Glyph bitmaps, glyph_count * glyph_size.y bytes in total.
     */
    const u8* data;

    /**
     * Size of every glyph, in pixels (x = width, y = height).
     */
    Size2u glyph_size;

    /**
     * Number of glyphs. Valid character codes go from 0 to
     * glyph_count - 1.
     */
    u32 glyph_count;

    /**
     * Bit ordering used within each glyph row.
     */
    FontBitOrder bit_order;
} BitmapFont;

/**
 * Returns the bitmap of a character.
 *
 * @param font Font to look the character up in.
 * @param character Character to look up, used as an unsigned code.
 * @return Pointer to the glyph's glyph_size.y bytes (one per row), or
 *         NULL if font is NULL or the character has no glyph.
 */
const u8* font_get_glyph(const BitmapFont* font, char character);

/**
 * Draws one character into the renderer backbuffer.
 *
 * Only the pixels that are set in the glyph are drawn, each one as a
 * scale x scale square; the background is left as it was. Nothing is
 * drawn if scale is 0 or the character has no glyph. Parts outside the
 * renderer are clipped. The result only reaches the screen after
 * renderer_present() (or renderer_present_rect()).
 *
 * @param font Font to draw with.
 * @param character Character to draw.
 * @param position Top-left corner of the character, in renderer
 *                 coordinates.
 * @param scale Integer scale factor (1 = native size).
 * @param color Color of the glyph pixels.
 */
void font_draw_char(const BitmapFont* font, char character, Vector2u position, u8 scale, Color color);

/**
 * Draws a string into the renderer backbuffer.
 *
 * Characters are drawn left to right, each one advancing the position
 * by the glyph width times scale. A '\n' returns to the starting x and
 * moves down one line (glyph height times scale). There is no
 * wrapping: text past the right edge is clipped. Other control
 * characters have no special handling and are drawn as ordinary
 * glyphs.
 *
 * @param font Font to draw with.
 * @param text NUL-terminated string. Nothing is drawn if NULL.
 * @param position Top-left corner of the first character, in renderer
 *                 coordinates.
 * @param scale Integer scale factor (1 = native size).
 * @param color Color of the glyph pixels.
 */
void font_draw_string(const BitmapFont* font, const char* text, Vector2u position, u8 scale, Color color);

#endif