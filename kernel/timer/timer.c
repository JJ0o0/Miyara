#include <timer/timer.h>
#include <math/math.h>

static u64 ticks = 0;
static u64 frequency = 0;

void timer_tick(void) { ticks++; }
u64 timer_get_ticks(void) { return ticks; }

void timer_set_frequency(u64 value) { frequency = value; }
u64 timer_get_frequency(void) { return frequency; }

u64 timer_ticks_from_ms(u64 milliseconds) {
    if (frequency == 0) {
        return 0;
    }

    u64 total = milliseconds * frequency;
    return ceil_div(total, 1000);
}