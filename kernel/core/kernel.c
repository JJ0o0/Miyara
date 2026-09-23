#include <terminal/terminal.h>
#include <interrupt/idt.h>

void kernel_main(void) {
    Terminal term = create_terminal();
    idt_init(&term);

    term_write(&term, "Ola Mundo!\n");

    while (1) {}
}