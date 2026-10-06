#include <graphics/font.h>

static const u8 test_font_data[128][8] = {
    ['A'] = {
        0b00111100,
        0b01100110,
        0b11000011,
        0b11000011,
        0b11111111,
        0b11000011,
        0b11000011,
        0b00000000
    }
};

const BitmapFont test_font = {
    (const u8*)test_font_data,
    {8, 8},
    128
};