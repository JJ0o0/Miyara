#include <graphics/font.h>
#include <graphics/renderer.h>

const u8* font_get_glyph(const BitmapFont* font, char character) {
    if (font == NULL) {
        return NULL;
    }

    u8 index = (u8)character;
    if (index >= font->glyph_count) {
        return NULL;
    }

    u32 bytes_per_glyph = font->glyph_size.y;
    u32 offset = (u32)index * bytes_per_glyph;

    return font->data + offset;
}

void font_draw_char(const BitmapFont* font, char character, Vector2u position, u8 scale, Color color) {
    if (scale == 0) {
        return;
    }
    
    const u8* glyph = font_get_glyph(font, character);
    if (glyph == NULL) {
        return;
    }

    for (u32 y = 0; y < font->glyph_size.y; y++) {
        u8 row = glyph[y];
        for (u32 x = 0; x < font->glyph_size.x; x++) {
            u8 mask;
            if (font->bit_order == FONT_BIT_ORDER_MSB_FIRST) {
                mask = (u8)(1 << (7 - x));
            } else {
                mask = (u8)(1 << x);
            }

            if (row & mask) {
                renderer_fill_rect(
                    (Vector2u){
                        position.x + (x * scale),
                        position.y + (y * scale)
                    },
                    (Size2u) {
                        scale,
                        scale
                    },
                    color
                );
            }
        }
    }
}

void font_draw_string(const BitmapFont* font, const char* text, Vector2u position, u8 scale, Color color) {
    if (font == NULL || text == NULL) {
        return;
    }
    
    Vector2u current = position;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            current.x = position.x;
            current.y += font->glyph_size.y * scale;
            continue;
        }

        font_draw_char(font, text[i], current, scale, color);
        current.x += font->glyph_size.x * scale;
    }
}