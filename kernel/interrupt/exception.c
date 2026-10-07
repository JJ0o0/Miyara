#include <interrupt/idt.h>
#include <log/log.h>

#include <graphics/framebuffer.h>
#include <graphics/color.h>

static void page_fault_handler(CPUContext* exception);
static const char* get_exception_name(u64 vector);
static const Color get_exception_framebuffer_color(u64 vector);

void exception_dispatch(u64 vector, CPUContext* exception) {
    const char* exceptionName = get_exception_name(vector);
    log(LOG_ERROR, exceptionName);

    framebuffer_clear(get_exception_framebuffer_color(vector));

    if (vector == 14) {
        page_fault_handler(exception);
        return;
    }

    log_hex(LOG_ERROR, "RIP", exception->rip);
    log_hex(LOG_ERROR, "CS", exception->cs);
    log_hex(LOG_ERROR, "RFLAGS", exception->rflags);
    log_hex(LOG_ERROR, "Error Code", exception->error_code);

    while (1) {}
}

static void page_fault_handler(CPUContext* exception) {
    log_hex(LOG_ERROR, "Fault Address", get_cr2());
    log_hex(LOG_ERROR, "Error Code", exception->error_code);

    while (1) {}
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

static const Color get_exception_framebuffer_color(u64 vector) {
    switch (vector) {
        case 0:     return (Color){255, 255, 0};
        case 6:     return (Color){255, 0, 255};
        case 13:    return (Color){255, 128, 0};
        case 14:    return (Color){255, 128, 0};
        default:    return (Color){0, 0, 255};
    }
}