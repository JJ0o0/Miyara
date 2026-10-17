#include <panic/panic.h>
#include <panic/panic_console.h>

#include <interrupt/idt.h>
#include <cpu/cpu.h>
#include <io/io.h>

static void panic_halt(void);

void panic(const char* message) {
    disable_interrupts();

    panic_console_clear();
    panic_console_write("MIYARA KERNEL PANIC\n\n");
    panic_console_write(message);
    panic_console_write("\n");

    panic_halt();
}

void panic_context(const char* message, CPUContext* context) {
    disable_interrupts();

    panic_console_clear();
    panic_console_write("MIYARA KERNEL PANIC\n\n");
    panic_console_write(message);
    panic_console_write("\n\n");

    if (context != NULL) {
        panic_console_write_hex("RIP", context->rip);
        panic_console_write_hex("RSP", context->rsp);
        panic_console_write_hex("CS", context->cs);
        panic_console_write_hex("SS", context->ss);
        panic_console_write_hex("CR2", get_cr2());
        panic_console_write_hex("RFLAGS", context->rflags);
        panic_console_write_hex("ERROR", context->error_code);
    }

    panic_halt();
}

static void panic_halt(void) {
    while (1) {
        asm volatile ("hlt");
    }
}