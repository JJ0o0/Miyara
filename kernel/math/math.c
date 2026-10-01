#include <math/math.h>

u64 ceil_div(u64 a, u64 b) {
    return (a / b) + (a % b != 0);
}

u64 align_up(u64 value, u64 alignment) {
    if (alignment == 0) {
        return value;
    }

    return ceil_div(value, alignment) * alignment;
}

u64 align_down(u64 value, u64 alignment) {
    if (alignment == 0) {
        return value;
    }

    return (value / alignment) * alignment;
}