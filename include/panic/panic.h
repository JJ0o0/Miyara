#ifndef PANIC_H
#define PANIC_H

#include <cpu/context.h>

/**
 * Halts the kernel with a fatal error message.
 *
 * Disables interrupts, switches to the panic framebuffer console
 * (separate from the normal terminal, so it works even if the
 * terminal or its state is what crashed), prints the message, and
 * then halts the CPU forever (hlt loop). Never returns.
 *
 * Use panic_context() instead when a CPUContext is available (e.g.
 * from an exception handler), since it also dumps register state.
 *
 * @param message Null-terminated message describing the failure.
 */
void panic(const char* message);

/**
 * Halts the kernel with a fatal error message and a register dump.
 *
 * Same as panic(), but additionally prints RIP, RSP, CS, SS, CR2,
 * RFLAGS and the error code from context, which helps diagnose a
 * crash caught by an exception handler. Never returns.
 *
 * @param message Null-terminated message describing the failure.
 * @param context CPU state at the time of the failure. If NULL, the
 *                register dump is skipped and only the message is
 *                printed.
 */
void panic_context(const char* message, CPUContext* context);

#endif