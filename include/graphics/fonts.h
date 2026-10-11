#ifndef FONTS_H
#define FONTS_H

#include <graphics/font.h>

/**
 * Built-in Terminus bitmap font.
 *
 * Generated from the Terminus 8x16 PSF font and embedded directly
 * into the kernel at build time.
 *
 * The font uses 8x16 pixel glyphs and is intended primarily for
 * terminal and text rendering.
 */
extern const BitmapFont terminus_8x16_font;

#endif