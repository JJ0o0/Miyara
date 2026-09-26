#ifndef IO_H
#define IO_H

#include <types/types.h>

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

#endif