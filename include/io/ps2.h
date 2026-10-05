#ifndef PS2_H
#define PS2_H

#include <types/types.h>

/**
 * Initializes the PS/2 controller.
 *
 * Runs the standard bring-up sequence using polling only (no timer or
 * IRQ needed):
 *  1. Disables both ports.
 *  2. Disables the port interrupts in the config byte and flushes the
 *     output buffer.
 *  3. Runs the controller self-test, then rewrites the config byte
 *     (some controllers reset it during the self-test).
 *  4. Tests the first port.
 *  5. Detects whether a second port exists, and tests it if so.
 *  6. Re-enables the ports that exist and turns their interrupts on.
 *
 * The translation bit of the config byte is left untouched, so the
 * scancode set seen by the keyboard driver is the one the controller
 * had on entry.
 *
 * Should be called once during boot, before interrupts are enabled:
 * the keyboard IRQ handler reads the data port and would race with
 * this polling code.
 *
 * Failures are not reported. The function returns at the first step
 * that fails, leaving the controller in whatever state it had reached
 * (typically with both ports disabled and interrupts off). Devices on
 * the ports are not reset or configured here.
 */
void ps2_init(void);

/**
 * Waits until the controller is ready to accept a byte.
 *
 * Polls the status register until the input buffer is empty. The
 * limit is a number of status reads, not a time, so it works without
 * a timer or interrupts.
 *
 * @return true if the controller became ready, false on timeout.
 */
bool ps2_wait_input(void);

/**
 * Waits until the controller has a byte for the CPU to read.
 *
 * Polls the status register until the output buffer is full, with the
 * same read-count limit as ps2_wait_input().
 *
 * @return true if a byte is available, false on timeout.
 */
bool ps2_wait_output(void);

/**
 * Sends a command byte to the controller (command port 0x64).
 *
 * Waits for the input buffer to be empty first. Commands that take an
 * argument or produce a response are followed by ps2_write_data() or
 * ps2_read_data().
 *
 * @param command Controller command byte.
 * @return true if the command was sent, false on timeout (nothing is
 *         written in that case).
 */
bool ps2_write_command(u8 command);

/**
 * Writes a byte to the data port (0x60).
 *
 * Waits for the input buffer to be empty first. Where the byte goes
 * depends on the previous command: it is the argument of a pending
 * controller command (e.g. the config byte after
 * ps2_write_command(0x60)), otherwise it is sent to the device on the
 * first port.
 *
 * @param data Byte to write.
 * @return true if the byte was written, false on timeout.
 */
bool ps2_write_data(u8 data);

/**
 * Reads a byte from the data port (0x60).
 *
 * Waits for the output buffer to be full first. The byte may be a
 * controller response or data from a device, and reading it removes it
 * from the buffer.
 *
 * @param data Output parameter; receives the byte on success and is
 *             left untouched on timeout. Must not be NULL.
 * @return true if a byte was read, false on timeout.
 */
bool ps2_read_data(u8* data);

/**
 * Reads a byte from the data port (0x60) without waiting.
 *
 * Unlike ps2_read_data(), this does not check the status register:
 * if the output buffer is empty, the value returned is meaningless.
 * Reading removes the byte from the buffer, as with any read of the
 * data port.
 *
 * Meant for interrupt handlers (e.g. the keyboard IRQ), where the
 * interrupt itself signals that a byte is waiting. Everywhere else,
 * use ps2_read_data(), or check ps2_data_available() first.
 *
 * @return Byte read from the data port.
 */
u8 ps2_read_data_now(void);

/**
 * Checks whether the controller has a byte waiting to be read.
 *
 * Reads only the status register, without waiting and without
 * consuming the byte.
 *
 * @return true if the output buffer is full, false if it is empty.
 */
bool ps2_data_available(void);

/**
 * Reads the controller configuration byte.
 *
 * Bit layout of the config byte:
 *  - bit 0: first port interrupt enabled (IRQ 1)
 *  - bit 1: second port interrupt enabled (IRQ 12)
 *  - bit 2: system flag (set after a successful POST)
 *  - bit 4: first port clock disabled
 *  - bit 5: second port clock disabled
 *  - bit 6: first port translation (scancode set 2 -> set 1)
 *  - bits 3 and 7: reserved, should read as 0
 *
 * A stale byte left in the output buffer would be read instead of the
 * config byte, so flush the buffer before calling this.
 *
 * @param config Output parameter; receives the config byte on
 *               success. Must not be NULL.
 * @return true on success, false if the command or the read timed
 *         out.
 */
bool ps2_read_config(u8* config);

/**
 * Writes the controller configuration byte.
 *
 * See ps2_read_config() for the bit layout. The usual pattern is to
 * read the current value, change only the bits needed and write it
 * back, so bits such as translation are preserved.
 *
 * @param config New config byte.
 * @return true on success, false on timeout.
 */
bool ps2_write_config(u8 config);

/**
 * Enables the first port (command 0xAE).
 *
 * Clears the "first port clock disabled" bit of the config byte.
 *
 * @return true if the command was sent, false on timeout.
 */
bool ps2_enable_first_port(void);

/**
 * Disables the first port (command 0xAD).
 *
 * Sets the "first port clock disabled" bit of the config byte. A
 * disabled port does not deliver data from its device.
 *
 * @return true if the command was sent, false on timeout.
 */
bool ps2_disable_first_port(void);

/**
 * Sends a byte to the device on the first port (e.g. a keyboard).
 *
 * Equivalent to ps2_write_data(); the device's reply has to be read
 * with ps2_read_data().
 *
 * @param data Byte to send to the device.
 * @return true if the byte was written, false on timeout.
 */
bool ps2_write_first_port(u8 data);

/**
 * Enables the second port (command 0xA8).
 *
 * Clears the "second port clock disabled" bit of the config byte. On
 * a controller with a second channel that bit then reads back as 0,
 * which is how its presence is detected; on a single-channel
 * controller it stays 1.
 *
 * @return true if the command was sent, false on timeout.
 */
bool ps2_enable_second_port(void);

/**
 * Disables the second port (command 0xA7).
 *
 * Sets the "second port clock disabled" bit of the config byte.
 *
 * @return true if the command was sent, false on timeout.
 */
bool ps2_disable_second_port(void);

/**
 * Sends a byte to the device on the second port (e.g. a mouse).
 *
 * Uses command 0xD4 to redirect the next data byte to the second
 * port. The device's reply has to be read with ps2_read_data().
 *
 * @param data Byte to send to the device.
 * @return true if the command and the byte were written, false on
 *         timeout.
 */
bool ps2_write_second_port(u8 data);

/**
 * Runs the controller self-test (command 0xAA).
 *
 * The controller answers 0x55 if it passed. On some hardware the
 * self-test also resets the config byte, so rewrite it afterwards.
 *
 * @return true only if the response was 0x55; false on timeout or any
 *         other response (the received value is not reported).
 */
bool ps2_controller_self_test(void);

/**
 * Tests the first port (command 0xAB).
 *
 * The controller answers 0x00 if the port passed. Any other value
 * means a fault in the port's clock or data line.
 *
 * @return true only if the response was 0x00; false on timeout or any
 *         other response.
 */
bool ps2_test_first_port(void);

/**
 * Tests the second port (command 0xA9).
 *
 * Same protocol as ps2_test_first_port(). Only meaningful if the
 * controller has a second channel.
 *
 * @return true only if the response was 0x00; false on timeout or any
 *         other response.
 */
bool ps2_test_second_port(void);

#endif