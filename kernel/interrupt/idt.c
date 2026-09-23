#include <terminal/terminal.h>
#include <interrupt/idt.h>

Terminal* global_terminal;
IDTTable idt_table = {0};

void idt_init(Terminal* terminal) {
    global_terminal = terminal;

    IDTR idtr;
    idtr.limit = (IDT_TABLE_ENTRY_COUNT * 16) - 1;
    idtr.base = (u64)idt_table;

    idt_set_gate(&idt_table[0], (u64)isr_divide_error);
    idt_set_gate(&idt_table[6], (u64)isr_invalid_opcode);

    idt_load(&idtr); // interrupt/idt.asm
}

void idt_set_handler(IDTEntry* entry, u64 handler) {
    entry->offset_low = handler & 0xFFFF;
    entry->offset_mid = (handler >> 16) & 0xFFFF;
    entry->offset_high = (handler >> 32) & 0xFFFFFFFF;
}

void idt_set_gate(IDTEntry* entry, u64 handler) {
    idt_set_handler(entry, handler);

    entry->selector = 0x18;
    entry->ist = 0;
    entry->type_attr = 0x8E;
    entry->reserved = 0;
}

void exception_dispatch(u64 vector, CPUContext* exception) {
    char* exceptionName = "Unknown Exception";
    switch (vector) {
        case 0:
            exceptionName = "Divide Error (#DE)";
            break;
        case 6:
            exceptionName = "Invalid Opcode (#UD)";
            break;
        default:
            break;
    }

    term_write(global_terminal, "EXCEPTION: ");
    term_write(global_terminal, exceptionName);
    term_putc(global_terminal, '\n');

    term_write(global_terminal, "RIP: 0x");
    term_write_hex(global_terminal, exception->rip);
    term_putc(global_terminal, '\n');

    term_write(global_terminal, "CS: 0x");
    term_write_hex(global_terminal, exception->cs);
    term_putc(global_terminal, '\n');

    term_write(global_terminal, "RFLAGS: 0x");
    term_write_hex(global_terminal, exception->rflags);
    term_putc(global_terminal, '\n');

    term_write(global_terminal, "Error Code: 0x");
    term_write_hex(global_terminal, exception->error_code);
    term_putc(global_terminal, '\n');

    while (1) {}
}
