#include <terminal/terminal.h>
#include <interrupt/idt.h>
#include <interrupt/pic.h>
#include <timer/pit.h>
#include <io/io.h>

void kernel_main(void) {
    Terminal term = create_terminal();
    term_write(&term, "[OK] Initialized terminal.\n");

    term_write(&term, "[WAIT] Initializing PIC...\n");
    pic_init();
    term_write(&term, "[OK] Initialized PIC.\n");

    term_write(&term, "[WAIT] Initializing IDT...\n");
    idt_init(&term);
    term_write(&term, "[OK] Initialized IDT.\n");

    term_write(&term, "[WAIT] Initializing PIT...\n");
    pit_init(100);
    term_write(&term, "[OK] Initialized PIT.\n");

    term_write(&term, "[WAIT] Enabling Interrupts...\n");
    enable_interrupts();
    term_write(&term, "[OK] Enabled Interrupts.\n");

    term_write(&term, "\nWelcome to Miyara\n");
    while (1) {}
}