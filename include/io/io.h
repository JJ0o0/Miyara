#ifndef IO_H
#define IO_H

#include <types/types.h>

/**
 * Reads a byte from an I/O port.
 *
 * Implemented in assembly (`in al, dx`).
 *
 * @param port I/O port to read from.
 * @return Byte read from the port.
 */
u8 io_in8(u16 port);

/**
 * Writes a byte to an I/O port.
 *
 * Implemented in assembly (`out dx, al`).
 *
 * @param port I/O port to write to.
 * @param value Byte to write.
 */
void io_out8(u16 port, u8 value);

/**
 * Enables maskable interrupts (`sti`).
 *
 * Should only be called after the IDT and PIC are fully
 * initialized, or unhandled/unmasked IRQs may fire.
 *
 * Implemented in assembly.
 */
void enable_interrupts(void);

/**
 * Disables maskable interrupts on the current CPU (cli).
 *
 * Clears the IF flag. The previous state is not remembered, so use
 * interrupts_save_and_disable() when it needs to be restored later.
 */
void disable_interrupts(void);

/**
 * Checks whether maskable interrupts are currently enabled.
 *
 * Reads the IF flag (bit 9) of RFLAGS.
 *
 * @return true if interrupts are enabled, false otherwise.
 */
bool interrupts_enabled(void);

/**
 * Saves the current interrupt state and disables interrupts.
 *
 * Opens a critical section that is safe to use even if interrupts
 * were already disabled (e.g. nested sections or inside an IRQ
 * handler). Must be paired with interrupts_restore(), passing back
 * the value stored in was_enabled.
 *
 * @param was_enabled Output parameter; set to true if interrupts
 *                    were enabled before the call, false if they
 *                    were already disabled. Must not be NULL.
 */
void interrupts_save_and_disable(bool* was_enabled);

/**
 * Restores the interrupt state saved by interrupts_save_and_disable().
 *
 * Enables interrupts only if was_enabled is true; otherwise they are
 * left disabled, so an outer critical section is not ended early.
 *
 * @param was_enabled Value obtained from interrupts_save_and_disable().
 */
void interrupts_restore(bool was_enabled);

#endif