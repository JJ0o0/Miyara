#include <panic/panic_console.h>

#include <graphics/framebuffer.h>
#include <graphics/fonts.h>
#include <graphics/font.h>
#include <graphics/color.h>

static u32 cursor_x = 8;
static u32 cursor_y = 8;

static const Color foreground = {255, 255, 255};
static const Color background = {0, 0, 0};

static void panic_console_write_char(char character);

void panic_console_clear(void) {
    framebuffer_clear(background);

    cursor_x = 8;
    cursor_y = 8;
}

void panic_console_write(const char* text) {
    if (text == NULL) {
        return;
    }

    for (u64 i = 0; text[i] != '\0'; i++) {
        panic_console_write_char(text[i]);
    }
}

void panic_console_write_hex(const char* label, u64 value) {
    static const char hex[] = "0123456789ABCDEF";

    if (label != NULL) {
        panic_console_write(label);
        panic_console_write(": ");
    }

    panic_console_write("0x");
    for (int shift = 60; shift >= 0; shift -= 4) {
        u8 digit = (value >> shift) & 0xF;
        panic_console_write_char(hex[digit]);
    }

    panic_console_write("\n");
}

static void panic_console_write_char(char character) {
    if (character == '\n') {
        cursor_x = 8;
        cursor_y += terminus_8x16_font.glyph_size.y;
        return;
    }

    const u8* glyph = font_get_glyph(&terminus_8x16_font, character);
    if (glyph == NULL) {
        return;
    }

    for (u32 y = 0; y < terminus_8x16_font.glyph_size.y; y++) {
        u8 row = glyph[y];
        for (u32 x = 0; x < terminus_8x16_font.glyph_size.x; x++) {
            u8 mask;
            if (terminus_8x16_font.bit_order == FONT_BIT_ORDER_MSB_FIRST) {
                mask = (u8)(1 << (7 - x));
            } else {
                mask = (u8)(1 << x);
            }

            if (row & mask) {
                framebuffer_put_pixel(cursor_x + x, cursor_y + y, foreground);
            }
        }
    }

    cursor_x += terminus_8x16_font.glyph_size.x;
}