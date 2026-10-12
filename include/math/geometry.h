#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <types/types.h>

/**
 * Insets applied to the four sides of a rectangular area, in pixels.
 *
 * Commonly used to describe internal padding while keeping the outer
 * rectangle unchanged.
 */
typedef struct {
    u32 left;
    u32 right;
    u32 top;
    u32 bottom;
} Insets;

#endif