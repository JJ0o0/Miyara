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

#endif