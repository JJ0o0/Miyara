#ifndef IDT_H
#define IDT_H

#include <terminal/terminal.h>
#include <types/types.h>

typedef struct {
    u16 offset_low;
    u16 selector;
    u8  ist;
    u8  type_attr;
    u16 offset_mid;
    u32 offset_high;
    u32 reserved;
} IDTEntry;

typedef struct __attribute__((packed)) {
    u16 limit;
    u64 base;
} IDTR;

#define IDT_TABLE_ENTRY_COUNT 256
typedef IDTEntry IDTTable[IDT_TABLE_ENTRY_COUNT];

typedef struct {
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

    u64 error_code;
    u64 rip;
    u64 cs;
    u64 rflags;
} CPUContext;

_Static_assert(sizeof(IDTEntry) == 16, "IDTEntry must be 16 bytes.");
_Static_assert(sizeof(IDTR) == 10, "IDTR must be 10 bytes.");
_Static_assert(sizeof(IDTTable) == 4096, "IDTTable must be 4096 bytes.");
_Static_assert(sizeof(CPUContext) == 152, "CPUContext must be 152 bytes.");

void idt_init(Terminal* terminal);
void idt_load(IDTR* idtr);
void idt_set_handler(IDTEntry* entry, u64 handler);
void idt_set_gate(IDTEntry* entry, u64 handler);

extern void isr_divide_error(void);
extern void isr_invalid_opcode(void);
void exception_dispatch(u64 vector, CPUContext* exception);

#endif