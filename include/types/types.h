#ifndef TYPES_H
#define TYPES_H

/**
 * Null pointer constant.
 *
 * Freestanding environment, so <stddef.h> is not used: NULL is
 * defined by hand, same as bool/true/false above.
 */
#define NULL ((void*)0)

/**
 * Fixed-width unsigned integer types.
 *
 * Freestanding environment, so <stdint.h> is not used: these are
 * defined directly on top of the built-in types, whose sizes are
 * fixed below by the _Static_assert checks.
 */
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

/**
 * Freestanding boolean type.
 *
 * No <stdbool.h> here, so true/false and bool are defined by hand.
 * Any nonzero value is truthy; only the literal 0 is false.
 */
#define true  1
#define false 0
typedef u8 bool;

typedef u64 size_t;

_Static_assert(sizeof(u8) == 1,   "u8 needs 1 byte."  );
_Static_assert(sizeof(u16) == 2,  "u16 needs 2 bytes.");
_Static_assert(sizeof(u32) == 4,  "u32 needs 4 bytes.");
_Static_assert(sizeof(u64) == 8,  "u64 needs 8 bytes.");

_Static_assert(sizeof(bool) == 1, "bool needs 1 byte.");
_Static_assert(sizeof(size_t) == 8, "size_t needs 8 bytes.");

#endif