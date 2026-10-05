#ifndef MATH_H
#define MATH_H

#include <types/types.h>

/**
 * Divides a by b, rounding up instead of truncating.
 *
 * Equivalent to ceil((double)a / b), computed without floating
 * point.
 *
 * @param a Dividend.
 * @param b Divisor. Must be nonzero.
 * @return Smallest integer >= a / b.
 */
u64 ceil_div(u64 a, u64 b);

/**
 * Rounds a value up to the nearest multiple of alignment.
 *
 * Values that are already aligned are returned unchanged. Computed
 * with division, so alignment does not need to be a power of two.
 *
 * @param value Value to round up.
 * @param alignment Multiple to align to. If 0, value is returned
 *                  unchanged.
 * @return Smallest multiple of alignment that is >= value.
 */
u64 align_up(u64 value, u64 alignment);

/**
 * Rounds a value down to the nearest multiple of alignment.
 *
 * Values that are already aligned are returned unchanged. Computed
 * with division, so alignment does not need to be a power of two.
 *
 * @param value Value to round down.
 * @param alignment Multiple to align to. If 0, value is returned
 *                  unchanged.
 * @return Largest multiple of alignment that is <= value.
 */
u64 align_down(u64 value, u64 alignment);

/**
 * Limits a value to the range [min, max].
 *
 * Returns min if value is below it, max if value is above it, and
 * value itself otherwise. Both bounds are inclusive.
 *
 * min must not be greater than max: with inverted bounds the result
 * is not a meaningful clamp (it depends on where value falls).
 *
 * @param value Value to limit.
 * @param min Lower bound of the range.
 * @param max Upper bound of the range.
 * @return value limited to the range [min, max].
 */
i32 clamp_i32(i32 value, i32 min, i32 max);

#endif