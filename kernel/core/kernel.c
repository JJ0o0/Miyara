#include <multiboot/multiboot.h>

#include <terminal/terminal.h>

#include <interrupt/idt.h>
#include <interrupt/pic.h>

#include <timer/timer.h>
#include <timer/pit.h>

#include <types/types.h>

#include <memory/paging.h>
#include <memory/heap.h>
#include <memory/pmm.h>

#include <log/log.h>
#include <cpu/cpu.h>

#include <io/ps2.h>
#include <io/io.h>

static void ps2_test(void) {
    log(LOG_DEBUG, "PS/2: testing controller communication...");
    ps2_write_command(0x20);

    u8 config;
    if (!ps2_read_data(&config)) {
        log(LOG_ERROR, "PS/2: failed to read controller configuration byte");
        return;
    }

    log_hex(LOG_DEBUG, "PS/2: controller configuration", config);
}

void kernel_main(u64 multiboot_info_address) {
    Terminal term = create_terminal();
    log_init(&term);

    log(LOG_SUCCESS, "Initialized terminal.");

    log(LOG_INFO, "Initializing PMM...");
    MBIHeader* mbi = (MBIHeader*)multiboot_info_address;
    pmm_init(mbi);
    log(LOG_SUCCESS, "Initialized PMM.");

    log(LOG_INFO, "Initializing Paging...");
    paging_init();
    log(LOG_SUCCESS, "Initialized Paging.");

    log(LOG_INFO, "Initializing Heap...");
    heap_init();
    log(LOG_SUCCESS, "Initialized Heap.");

    log(LOG_INFO, "Initializing PIC...");
    pic_init();
    log(LOG_SUCCESS, "Initialized PIC.");

    log(LOG_INFO, "Initializing IDT...");
    idt_init();
    log(LOG_SUCCESS, "Initialized IDT.");

    log(LOG_INFO, "Initializing CPU...");
    cpu_init();
    log(LOG_SUCCESS, "Initialized CPU.");

    log(LOG_INFO, "Initializing PIT...");
    pit_init(100);
    log(LOG_SUCCESS, "Initialized PIT.");

    log(LOG_INFO, "Enabling Interrupts...");
    enable_interrupts();
    log(LOG_SUCCESS, "Enabled Interrupts.");
    
    term_write(&term, "\nWelcome to Miyara\n");

    while (1) {}
}