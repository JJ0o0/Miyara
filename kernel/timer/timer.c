#include <timer/timer.h>

static u64 ticks = 0;

void timer_tick(void) { ticks++; }
u64 timer_get_ticks(void) { return ticks; }

void timer_wait_ticks(u64 ticks_to_wait) {
    u64 start = timer_get_ticks();
    u64 dest = start + ticks_to_wait;

    while (timer_get_ticks() < dest) {}
}
