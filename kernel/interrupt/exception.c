#include <interrupt/idt.h>

#include <panic/panic_console.h>
#include <panic/panic.h>

static const char* get_exception_name(u64 vector);

void exception_dispatch(u64 vector, CPUContext* exception) {
    if (vector == 14) {
        panic_console_write_hex("CR2", get_cr2());
    }

    panic_context(get_exception_name(vector), exception);
}

static const char* get_exception_name(u64 vector) {
    switch (vector) {
        case 0:     return "Divide Error (#DE)";
        case 6:     return "Invalid Opcode (#UD)";
        case 13:    return "General Protection (#GP)";
        case 14:    return "Page Fault (#PF)";
        default:    return "Unknown Exception";
    }
}