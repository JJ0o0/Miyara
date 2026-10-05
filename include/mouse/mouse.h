#ifndef MOUSE_H
#define MOUSE_H

#include <types/types.h>

/**
 * State of the mouse buttons at the time of a packet.
 *
 * Each field is true while the button is held down, not only on the
 * packet where it was pressed.
 */
typedef struct {
    bool left;
    bool right;
    bool middle;
} MouseButtons;

/**
 * Movement and button state decoded from one PS/2 mouse packet.
 */
typedef struct {
    /**
     * Button state when the packet was sent.
     */
    MouseButtons buttons;

    /**
     * Horizontal movement since the previous packet, in mouse counts.
     * Positive is to the right.
     */
    i16 dx;

    /**
     * Vertical movement since the previous packet, in mouse counts.
     * Positive is up, as reported by the mouse, which is the opposite
     * of screen coordinates: invert it when moving a cursor.
     */
    i16 dy;
} MouseEvent;

/**
 * Initializes the PS/2 mouse on the controller's second port.
 *
 * Resets the device and checks its self-test result (0xAA) and ID
 * (0x00, standard 3-byte mouse), then enables data reporting so the
 * mouse starts sending packets.
 *
 * Requires ps2_init() to have run successfully on a controller with a
 * second port. Must be called with interrupts disabled: the replies
 * are read by polling, and the mouse IRQ handler would otherwise
 * consume those bytes.
 *
 * Failures are not reported. On the first step that fails the
 * function returns and the mouse stays disabled.
 */
void mouse_init(void);

/**
 * Handles one byte of the mouse data stream.
 *
 * Meant to be called from the mouse IRQ handler (IRQ 12), once per
 * interrupt: the mouse raises one interrupt for each byte of a
 * packet. The byte is read from the data port without waiting, so
 * this must only be called when a byte is pending.
 *
 * Standard packets have 3 bytes, accumulated across calls:
 *  - byte 0: flags. bit 0 left button, bit 1 right button, bit 2
 *    middle button, bit 3 always 1, bit 4 X sign, bit 5 Y sign,
 *    bit 6 X overflow, bit 7 Y overflow.
 *  - byte 1: X movement (low 8 bits, sign in byte 0).
 *  - byte 2: Y movement (low 8 bits, sign in byte 0).
 *
 * A byte that arrives at the start of a packet without bit 3 set is
 * dropped, to resynchronize after a lost byte. Packets with an
 * overflow bit set are discarded. Every other complete packet is
 * pushed to the event queue as an EVENT_MOUSE event.
 */
void mouse_handle(void);

#endif