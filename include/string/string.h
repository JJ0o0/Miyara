#ifndef STRING_H
#define STRING_H

#include <types/types.h>

/**
 * Compares two NUL-terminated strings for equality.
 *
 * @param a First string.
 * @param b Second string.
 * @return true if both strings have exactly the same characters and
 *         the same length; false if they differ or if either one is
 *         NULL (so two NULL pointers are not considered equal).
 */
static inline bool string_equals(const char* a, const char* b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    size_t i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return false;
        }

        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

/**
 * Copies a NUL-terminated string into a destination buffer.
 *
 * The terminating NUL byte is copied as well.
 *
 * The destination buffer must be large enough to hold the entire
 * source string including its terminating NUL character.
 *
 * @param src Source NUL-terminated string.
 * @param dest Destination buffer.
 *
 * @note Does nothing if src or dest is NULL.
 */
static inline void string_copy(const char* src, char* dest) {
    if (src == NULL || dest == NULL) {
        return;
    }

    size_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

/**
 * Returns the length of a NUL-terminated string.
 *
 * The terminating NUL character is not included in the returned
 * length.
 *
 * @param str NUL-terminated string.
 *
 * @return Number of characters before the terminating NUL.
 * @return 0 if str is NULL or points to an empty string.
 */
static inline size_t string_len(const char* str) {
    if (str == NULL) {
        return 0;
    }

    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }

    return len;
}

#endif