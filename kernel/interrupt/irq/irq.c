#include <terminal/terminal.h>
#include <interrupt/idt.h>
#include <interrupt/pic.h>

extern Terminal* global_terminal;

void irq_dispatch(u64 irq) {
    term_write(global_terminal, "IRQ: ");
    term_write_hex(global_terminal, irq);
    term_putc(global_terminal, '\n');

    pic_send_eoi(irq);
}