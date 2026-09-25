#include <terminal/terminal.h>
#include <interrupt/idt.h>
#include <interrupt/pic.h>
#include <timer/pit.h>
#include <log/log.h>
#include <io/io.h>

void kernel_main(void) {
    Terminal term = create_terminal();
    log_init(&term);

    log(LOG_SUCCESS, "Initialized terminal.");

    log(LOG_INFO, "Initializing PIC...");
    pic_init();
    log(LOG_SUCCESS, "Initialized PIC.");

    log(LOG_INFO, "Initializing IDT...");
    idt_init();
    log(LOG_SUCCESS, "Initialized IDT.");

    log(LOG_INFO, "Initializing PIT...");
    pit_init(100);
    log(LOG_SUCCESS, "Initialized PIT.");

    log(LOG_INFO, "Enabling Interrupts...");
    enable_interrupts();
    log(LOG_SUCCESS, "Enabled Interrupts.");

    log(LOG_INFO, "\nWelcome to Miyara\n");
    while (1) {}
}