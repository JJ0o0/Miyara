#include <terminal/terminal.h>
#include <interrupt/idt.h>

extern void test_invalid_opcode(void);

void kernel_main(void) {
    Terminal term = create_terminal();
    idt_init(&term);

    int res = 10 / 0;
    term_write_int(&term, res);
    //test_invalid_opcode();

    while (1) {}
}