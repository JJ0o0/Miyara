#ifndef CONTEXT_H
#define CONTEXT_H

#include <types/types.h>

/**
 * CPU state saved on the stack by interrupt and exception stubs.
 *
 * The layout must match the frame built by the ISR stubs.
 * Fields are ordered exactly as they appear in memory, from
 * r15 at the lowest address through rflags at the highest.
 *
 * The same layout is used by exceptions and hardware interrupts.
 * Exceptions that do not provide a CPU error code, and all IRQs,
 * use an artificial error code of 0.
 *
 * Changes made to this structure before returning from the handler
 * are restored to the CPU by iretq.
 */
typedef struct {
    /**
     * General-purpose registers, saved by the ISR stubs.
     */
    u64 r15;
    u64 r14;
    u64 r13;
    u64 r12;
    u64 r11;
    u64 r10;
    u64 r9;
    u64 r8;

    u64 rbp;
    u64 rdi;
    u64 rsi;
    u64 rdx;
    u64 rcx;
    u64 rbx;
    u64 rax;

    /**
     * Interrupt or exception error code.
     *
     * Exceptions that provide an error code (#GP, #PF) use the value
     * pushed by the CPU. Other exceptions and hardware interrupts use
     * an artificial 0 pushed by their ISR stub.
     */
    u64 error_code;

    /**
     * Instruction pointer at the time of the interrupt or exception.
     * Pushed by the CPU.
     */
    u64 rip;
    
    /**
     * Code segment at the time of the interrupt or exception.
     * Pushed by the CPU.
     */
    u64 cs;

    /**
     * CPU flags at the time of the interrupt or exception.
     * Pushed by the CPU.
     */
    u64 rflags;

    u64 rsp;
    u64 ss;
} CPUContext;

_Static_assert(sizeof(CPUContext) == 168, "CPUContext must be 168 bytes.");

#endif