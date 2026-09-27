#ifndef TYPES_H
#define TYPES_H

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

_Static_assert(sizeof(u8) == 1,   "u8 needs 1 byte."  );
_Static_assert(sizeof(u16) == 2,  "u16 needs 2 bytes.");
_Static_assert(sizeof(u32) == 4,  "u32 needs 4 bytes.");
_Static_assert(sizeof(u64) == 8,  "u64 needs 8 bytes.");

_Static_assert(sizeof(bool) == 1, "bool needs 1 byte.");

#endif