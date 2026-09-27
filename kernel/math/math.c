#include <math/math.h>

u64 ceil_div(u64 a, u64 b) {
    return (a + b - 1) / b;
}