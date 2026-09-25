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

/**
 * Busy-waits until the given number of ticks has elapsed.
 *
 * Spins on timer_get_ticks() rather than sleeping, so it burns CPU
 * time for the whole wait. Interrupts must be enabled for the tick
 * counter to advance while waiting.
 *
 * @param ticks_to_wait Number of ticks to wait for.
 */
void timer_wait_ticks(u64 ticks_to_wait);

#endif