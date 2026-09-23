#include <terminal/terminal.h>
#include <interrupt/idt.h>

void kernel_main(void) {
    Terminal term = create_terminal();
    idt_init(&term);

    int numerador = 67;
    int denominador = 0;
    int resultado = numerador / denominador;

    term_write_int(&term, resultado);
    term_putc(&term, '\n');

    while (1) {}
}