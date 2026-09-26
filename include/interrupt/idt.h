#ifndef IDT_H
#define IDT_H

#include <types/types.h>

/**
 * A single 64-bit IDT gate descriptor (16 bytes).
 *
 * The handler address is split across three fields:
 * offset_low (bits 0-15), offset_mid (bits 16-31) and
 * offset_high (bits 32-63).
 */
typedef struct {
    /**
     * Handler address, bits 0-15.
     */
    u16 offset_low;

    /**
     * Code segment selector loaded into CS when the handler runs.
     */
    u16 selector;

     /**
     * Interrupt Stack Table index (bits 0-2).
     *
     * 0 means no stack switch. Bits 3-7 are reserved and must be zero.
     */
    u8  ist;

    /**
     * Gate type and attributes.
     *
     * Bit 7: present (P). Bits 5-6: privilege level (DPL).
     * Bit 4: must be zero. Bits 0-3: gate type
     * (0xE = interrupt gate, 0xF = trap gate).
     */
    u8  type_attr;

    /**
     * Handler address, bits 16-31.
     */
    u16 offset_mid;

    /**
     * Handler address, bits 32-63.
     */
    u32 offset_high;

    /**
     * Reserved. Must be zero.
     */
    u32 reserved;
} IDTEntry;

/**
 * Descriptor loaded into the IDTR register by `lidt`.
 */
typedef struct __attribute__((packed)) {
    /**
     * Size of the IDT in bytes, minus one.
     */
    u16 limit;

    /**
     * Linear address of the first IDT entry.
     */
    u64 base;
} IDTR;

/**
 * Number of vectors supported by the IDT.
 */
#define IDT_TABLE_ENTRY_COUNT 256

/**
 * A full IDT: one gate descriptor per vector.
 */
typedef IDTEntry IDTTable[IDT_TABLE_ENTRY_COUNT];

/**
 * CPU state saved on the stack when an exception occurs.
 *
 * The layout must match the frame built by the ISR stubs
 * (see idt.asm): the fields are in memory order, from the lowest
 * address (r15) to the highest (rflags).
 *
 * Changes made to this structure by the C handler are applied to
 * the CPU when the exception returns (e.g. changing rip).
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
     * Exception error code.
     *
     * Pushed by the CPU for exceptions that provide one (#GP, #PF).
     * For the others, the stub pushes an artificial 0.
     */
    u64 error_code;

    /**
     * Instruction pointer at the time of the exception.
     * Pushed by the CPU.
     */
    u64 rip;
    
    /**
     * Code segment at the time of the exception.
     * Pushed by the CPU.
     */
    u64 cs;

    /**
     * CPU flags at the time of the exception.
     * Pushed by the CPU.
     */
    u64 rflags;
} CPUContext;

_Static_assert(sizeof(IDTEntry) == 16, "IDTEntry must be 16 bytes.");
_Static_assert(sizeof(IDTR) == 10, "IDTR must be 10 bytes.");
_Static_assert(sizeof(IDTTable) == 4096, "IDTTable must be 4096 bytes.");
_Static_assert(sizeof(CPUContext) == 152, "CPUContext must be 152 bytes.");

/**
 * Initializes the IDT, registers the exception handlers and loads
 * the table into the CPU.
 */
void idt_init(void);

/**
 * Loads the given descriptor into the IDTR register (`lidt`).
 *
 * Implemented in assembly.
 *
 * @param idtr Pointer to the IDTR descriptor (limit and base).
 */
void idt_load(IDTR* idtr);

/**
 * Sets the handler address of an IDT entry.
 *
 * Splits the address into the offset_low, offset_mid and
 * offset_high fields.
 *
 * @param entry IDT entry to modify.
 * @param handler Address of the handler routine.
 */
void idt_set_handler(IDTEntry* entry, u64 handler);

/**
 * Configures an IDT entry as a gate for the given handler.
 *
 * @param entry IDT entry to configure.
 * @param handler Address of the handler routine.
 */
void idt_set_gate(IDTEntry* entry, u64 handler);

/**
 * Reads the CR2 register.
 *
 * After a page fault, CR2 holds the linear address that caused it.
 *
 * Implemented in assembly.
 *
 * @return Value of CR2.
 */
extern u64 get_cr2(void);

/**
 * ISR stub for #DE (Divide Error, vector 0). No CPU error code.
 */
extern void isr_divide_error(void);

/**
 * ISR stub for #UD (Invalid Opcode, vector 6). No CPU error code.
 */
extern void isr_invalid_opcode(void);

/**
 * ISR stub for #GP (General Protection Fault, vector 13).
 * The CPU pushes an error code.
 */
extern void isr_general_protection(void);

/**
 * ISR stub for #PF (Page Fault, vector 14).
 * The CPU pushes an error code. The faulting address is in CR2.
 */
extern void isr_page_fault(void);

/**
 * Common C entry point for all exceptions.
 *
 * Called by common_exception_handler (assembly) after the ISR stub
 * has saved the CPU state.
 *
 * @param vector Exception vector number (0 = #DE, 6 = #UD, 13 = #GP, 14 = #PF).
 * @param exception Pointer to the saved CPU state on the stack.
 */
void exception_dispatch(u64 vector, CPUContext* exception);

/**
 * ISR stub for IRQ 0 (timer / PIT).
 *
 * Unlike the exception stubs, this one does not push an error code
 * or a vector number, and it does not go through
 * common_exception_handler: it saves the registers, calls
 * irq_dispatch(0) directly and restores them before iretq.
 */
extern void isr_timer(void);

extern void isr_keyboard(void);

/**
 * Common C entry point for hardware interrupts (IRQs).
 *
 * Called by the ISR stub after it has saved the general-purpose
 * registers. Unlike exception_dispatch, it does not receive a
 * CPUContext*, since isr_timer does not build one on the stack.
 *
 * @param irq IRQ number (0 = timer/PIT).
 */
void irq_dispatch(u64 irq);

#endif