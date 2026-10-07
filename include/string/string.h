#ifndef STRING_H
#define STRING_H

#include <types/types.h>

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

#endif