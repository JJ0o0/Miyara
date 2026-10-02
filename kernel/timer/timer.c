#include <timer/timer.h>

static u64 ticks = 0;

void timer_tick(void) { ticks++; }
u64 timer_get_ticks(void) { return ticks; }