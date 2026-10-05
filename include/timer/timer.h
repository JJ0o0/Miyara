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
 * Records the tick rate, in Hz, used by timer_ticks_from_ms().
 *
 * Called by pit_init() with the frequency it programs. This only
 * stores the value: it does not reprogram the PIT.
 *
 * @param value Tick rate in Hz.
 */
void timer_set_frequency(u64 value);

/**
 * Returns the tick rate recorded by timer_set_frequency().
 *
 * @return Tick rate in Hz, or 0 if no frequency has been set yet
 *         (pit_init() has not run).
 */
u64 timer_get_frequency(void);

/**
 * Converts a duration in milliseconds to a number of timer ticks.
 *
 * The result is rounded up, so any non-zero duration is at least one
 * tick. Because counting normally starts in the middle of a tick, a
 * wait of that many ticks can end up to one tick shorter than the
 * requested time.
 *
 * Ticks only advance while the timer IRQ is being delivered, so do
 * not use this to bound code that runs with interrupts disabled.
 *
 * @param milliseconds Duration in milliseconds.
 * @return Equivalent number of ticks, or 0 if the frequency has not
 *         been set yet (timer_get_frequency() == 0).
 */
u64 timer_ticks_from_ms(u64 milliseconds);

#endif