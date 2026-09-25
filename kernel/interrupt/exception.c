#include <terminal/terminal.h>
#include <interrupt/idt.h>

extern Terminal* global_terminal;

void exception_dispatch(u64 vector, CPUContext* exception) {
    const char* exceptionName = "Unknown Exception";
    switch (vector) {
        case 0:
            exceptionName = "Divide Error (#DE)";
            break;
        case 6:
            exceptionName = "Invalid Opcode (#UD)";
            break;
        case 13:
            exceptionName = "General Protection (#GP)";
            break;
        case 14:
            exceptionName = "Page Fault (#PF)";
            break;
        default:
            break;
    }

    term_write(global_terminal, "EXCEPTION: ");
    term_write(global_terminal, exceptionName);
    term_putc(global_terminal, '\n');

    if (vector == 14) {
        term_write(global_terminal, "Fault Address: 0x");
        term_write_hex(global_terminal, get_cr2());
        term_putc(global_terminal, '\n');
    }

    term_write(global_terminal, "RIP: 0x");
    term_write_hex(global_terminal, exception->rip);
    term_putc(global_terminal, '\n');

    term_write(global_terminal, "CS: 0x");
    term_write_hex(global_terminal, exception->cs);
    term_putc(global_terminal, '\n');

    term_write(global_terminal, "RFLAGS: 0x");
    term_write_hex(global_terminal, exception->rflags);
    term_putc(global_terminal, '\n');

    term_write(global_terminal, "Error Code: 0x");
    term_write_hex(global_terminal, exception->error_code);
    term_putc(global_terminal, '\n');

    while (1) {}
}