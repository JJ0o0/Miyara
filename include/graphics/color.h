#ifndef COLOR_H
#define COLOR_H

#include <types/types.h>

/**
 * An RGB color with 8 bits per channel.
 *
 * Independent of any pixel format: functions that draw it, such as
 * framebuffer_put_pixel(), convert it to the layout they need. There
 * is no alpha channel.
 */
typedef struct {
    /**
     * Red intensity, from 0 (none) to 255 (full).
     */
    u8 r;

    /**
     * Green intensity, from 0 (none) to 255 (full).
     */
    u8 g;

    /**
     * Blue intensity, from 0 (none) to 255 (full).
     */
    u8 b;
} Color;

#endif