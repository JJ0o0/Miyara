#ifndef PIT_H
#define PIT_H

#include <types/types.h>

/**
 * Initializes the PIT (Programmable Interval Timer, channel 0) to
 * fire at the given frequency.
 *
 * Configures channel 0 in mode 3 (square wave generator) and loads
 * the divisor derived from the PIT's base frequency (~1.193182 MHz).
 * Channel 0 is wired to IRQ 0, so once the PIC is initialized and
 * IRQs are unmasked, this produces a periodic timer interrupt.
 *
 * @param frequency Desired interrupt frequency, in Hz.
 */
void pit_init(u16 frequency);

#endif