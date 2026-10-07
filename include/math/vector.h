#ifndef VECTOR_H
#define VECTOR_H

#include <types/types.h>

/**
 * Represents a two-dimensional signed integer vector.
 *
 * This type is suitable for positions, offsets, deltas, and other values
 * that may become negative during intermediate calculations.
 *
 * Typical use cases include cursor positions before clamping, movement
 * deltas, and signed coordinate arithmetic.
 */
typedef struct {
    i32 x;
    i32 y;
} Vector2i;

/**
 * Represents a two-dimensional unsigned integer vector.
 *
 * This type is suitable for coordinates and values that are guaranteed to
 * be non-negative.
 *
 * Typical use cases include framebuffer positions, renderer coordinates,
 * dimensions, and other screen-space values.
 */
typedef struct {
    u32 x;
    u32 y;
} Vector2u;

/**
 * Represents a two-dimensional unsigned size.
 *
 * Size2u is an alias of Vector2u used when the values represent dimensions
 * rather than a position.
 *
 * The x component represents the width and the y component represents the
 * height.
 */
typedef Vector2u Size2u;

static inline Vector2u vector2u_add(Vector2u a, Vector2u b) {
    return (Vector2u){
        a.x + b.x,
        a.y + b.y
    };
}

#endif