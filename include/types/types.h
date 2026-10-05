#ifndef TYPES_H
#define TYPES_H

/**
 * Null pointer constant.
 *
 * Freestanding environment, so <stddef.h> is not used: NULL is
 * defined by hand, same as bool/true/false below.
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
 * Fixed-width signed integer types.
 *
 * Same sizes as their unsigned counterparts (u8..u64), also checked
 * by the _Static_assert checks below. Values use two's complement,
 * so the range of iN is -2^(N-1) to 2^(N-1) - 1.
 */
typedef signed   char   i8;
typedef signed   short  i16;
typedef signed   int    i32;
typedef signed   long   i64;

/**
 * Freestanding boolean type.
 *
 * No <stdbool.h> here, so true/false and bool are defined by hand.
 * Any nonzero value is truthy; only the literal 0 is false.
 */
#define true  1
#define false 0
typedef u8 bool;

/**
 * Freestanding size type, used for sizes and counts of bytes (e.g.
 * mem_set's quantity parameter).
 *
 * No <stddef.h> here, so size_t is defined by hand as an alias for
 * u64, fixed at 64-bit for this platform.
 */
typedef u64 size_t;

_Static_assert(sizeof(u8) == 1,   "u8 needs 1 byte."  );
_Static_assert(sizeof(u16) == 2,  "u16 needs 2 bytes.");
_Static_assert(sizeof(u32) == 4,  "u32 needs 4 bytes.");
_Static_assert(sizeof(u64) == 8,  "u64 needs 8 bytes.");

_Static_assert(sizeof(i8) == 1,  "i8 needs 1 byte.");
_Static_assert(sizeof(i16) == 2, "i16 needs 2 bytes.");
_Static_assert(sizeof(i32) == 4, "i32 needs 4 bytes.");
_Static_assert(sizeof(i64) == 8, "i64 needs 8 bytes.");

_Static_assert(sizeof(bool) == 1, "bool needs 1 byte.");
_Static_assert(sizeof(size_t) == 8, "size_t needs 8 bytes.");

#endif