#include <interrupt/idt.h>
#include <log/log.h>

const char* get_exception_name(u64 vector);

void exception_dispatch(u64 vector, CPUContext* exception) {
    const char* exceptionName = get_exception_name(vector);
    log(LOG_ERROR, exceptionName);

    if (vector == 14) {
        log_hex(LOG_ERROR, "Fault Address", get_cr2());
    }

    log_hex(LOG_ERROR, "RIP", exception->rip);
    log_hex(LOG_ERROR, "CS", exception->cs);
    log_hex(LOG_ERROR, "RFLAGS", exception->rflags);
    log_hex(LOG_ERROR, "Error Code", exception->error_code);

    while (1) {}
}

const char* get_exception_name(u64 vector) {
    switch (vector) {
        case 0:     return "Divide Error (#DE)";
        case 6:     return "Invalid Opcode (#UD)";
        case 13:    return "General Protection (#GP)";
        case 14:    return "Page Fault (#PF)";
        default:    return "Unknown Exception";
    }
}