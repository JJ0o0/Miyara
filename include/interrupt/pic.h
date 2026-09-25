#ifndef PIC_H
#define PIC_H

#include <types/types.h>

/**
 * I/O port for sending commands to the master PIC (IRQ 0-7).
 */
#define PIC_MASTER_COMMAND 0x20

/**
 * I/O port for reading/writing the master PIC's data register
 * (used to set its vector offset and IRQ mask).
 */
#define PIC_MASTER_DATA 0x21

/**
 * I/O port for sending commands to the slave PIC (IRQ 8-15).
 */
#define PIC_SLAVE_COMMAND 0xA0

/**
 * I/O port for reading/writing the slave PIC's data register
 * (used to set its vector offset and IRQ mask).
 */
#define PIC_SLAVE_DATA 0xA1

/**
 * Initializes the master and slave 8259 PICs.
 *
 * Remaps their interrupt vectors so IRQs no longer overlap CPU
 * exception vectors, and configures the master/slave cascade.
 */
void pic_init(void);

/**
 * Sends an End of Interrupt (EOI) to the PIC(s).
 *
 * Must be called at the end of every IRQ handler, or the PIC will
 * not deliver further interrupts. If the IRQ came from the slave
 * PIC (irq >= 8), the EOI must be sent to both the slave and the
 * master.
 *
 * @param irq IRQ number that was handled (0-15).
 */
void pic_send_eoi(u8 irq);

#endif