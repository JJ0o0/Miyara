#include <interrupt/idt.h>
#include <interrupt/pic.h>
#include <log/log.h>

IDTTable idt_table = {0};

void idt_init(void) {
    IDTR idtr;
    idtr.limit = (IDT_TABLE_ENTRY_COUNT * 16) - 1;
    idtr.base = (u64)idt_table;

    idt_set_gate(&idt_table[0], (u64)isr_divide_error);
    idt_set_gate(&idt_table[6], (u64)isr_invalid_opcode);
    idt_set_gate(&idt_table[13], (u64)isr_general_protection);
    idt_set_gate(&idt_table[14], (u64)isr_page_fault);

    idt_set_gate(&idt_table[32], (u64)isr_timer);
    idt_set_gate(&idt_table[33], (u64)isr_keyboard);

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