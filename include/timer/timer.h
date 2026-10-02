#ifndef TIMER_H
#define TIMER_H

#include <types/types.h>

/**
 * Advances the tick counter by one.
 *
 * Meant to be called from the timer IRQ handler (irq_dispatch),
 * once per PIT interrupt.
 */
void timer_tick(void);

/**
 * Returns the number of ticks elapsed since boot.
 *
 * The tick rate depends on the frequency passed to pit_init.
 *
 * @return Current tick count.
 */
u64 timer_get_ticks(void);

#endif