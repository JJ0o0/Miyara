#ifndef MATH_H
#define MATH_H

#include <types/types.h>

/**
 * Divides a by b, rounding the result up instead of truncating.
 *
 * Equivalent to ceil((double)a / b), but computed using integer
 * arithmetic only.
 *
 * @param a Dividend.
 * @param b Divisor. Must be nonzero.
 *
 * @return Smallest integer greater than or equal to a / b.
 */
u64 ceil_div(u64 a, u64 b);

/**
 * Rounds a value up to the nearest multiple of alignment.
 *
 * If value is already aligned, it is returned unchanged.
 * The operation uses integer division, so alignment does not need
 * to be a power of two.
 *
 * If alignment is 0, value is returned unchanged.
 *
 * @param value Value to round up.
 * @param alignment Multiple to align to.
 *
 * @return Smallest multiple of alignment greater than or equal to
 *         value, or value itself if alignment is 0.
 */
u64 align_up(u64 value, u64 alignment);

/**
 * Rounds a value down to the nearest multiple of alignment.
 *
 * If value is already aligned, it is returned unchanged.
 * The operation uses integer division, so alignment does not need
 * to be a power of two.
 *
 * If alignment is 0, value is returned unchanged.
 *
 * @param value Value to round down.
 * @param alignment Multiple to align to.
 *
 * @return Largest multiple of alignment less than or equal to value,
 *         or value itself if alignment is 0.
 */
u64 align_down(u64 value, u64 alignment);

/**
 * Clamps a signed 32-bit integer to an inclusive range.
 *
 * Returns min if value is smaller than min, max if value is greater
 * than max, and value itself otherwise.
 *
 * min must not be greater than max.
 *
 * @param value Value to clamp.
 * @param min Inclusive lower bound.
 * @param max Inclusive upper bound.
 *
 * @return value limited to the range [min, max].
 */
i32 clamp_i32(i32 value, i32 min, i32 max);

/**
 * Clamps a signed 64-bit integer to an inclusive range.
 *
 * Returns min if value is smaller than min, max if value is greater
 * than max, and value itself otherwise.
 *
 * min must not be greater than max.
 *
 * @param value Value to clamp.
 * @param min Inclusive lower bound.
 * @param max Inclusive upper bound.
 *
 * @return value limited to the range [min, max].
 */
i64 clamp_i64(i64 value, i64 min, i64 max);

#endif