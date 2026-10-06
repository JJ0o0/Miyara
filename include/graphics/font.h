#ifndef FONT_H
#define FONT_H

#include <graphics/color.h>
#include <types/types.h>
#include <math/vector.h>

typedef struct {
    const u8* data;
    Size2u glyph_size;
    u32 glyph_count;
} BitmapFont;

const u8* font_get_glyph(const BitmapFont* font, char character);
void font_draw_char(const BitmapFont* font, char character, Vector2u position, Color color);
void font_draw_string(const BitmapFont* font, const char* text, Vector2u position, Color color);

#endif