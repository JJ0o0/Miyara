#include <terminal/terminal.h>

void kernel_main(void) {
    Terminal term = create_terminal();

    for (int i = 0; i < 15; i++) {
        term_write(&term, "====================\n");
        term_write(&term, "Miyara Kernel IT ");
        term_write_int(&term, i);
        term_putc(&term, '\n');
        
        term_write(&term, "Decimal: ");
        term_write_int(&term, 12345);
        term_putc(&term, '\n');

        term_write(&term, "Negative: ");
        term_write_int(&term, -420);
        term_putc(&term, '\n');

        term_write(&term, "Zero: ");
        term_write_int(&term, 0);
        term_putc(&term, '\n');

        term_write(&term, "Hex: ");
        term_write_hex(&term, 0xDEADBEEF);
        term_putc(&term, '\n');
        term_write(&term, "====================\n");
    }

    while (1) {}
}