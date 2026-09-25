#include <log/log.h>

static Terminal* terminal;

void log_init(Terminal *term) {
    terminal = term;
}

const char* get_type_as_string(LogType type);

void log(LogType type, const char* msg) {
    const char* typeStr = get_type_as_string(type);
    
    term_write(terminal, "[");
    term_write(terminal, typeStr);
    term_write(terminal, "] ");
    term_write(terminal, msg);
    term_putc(terminal, '\n');
}

void log_hex(LogType type, const char* label, u64 value) {
    const char* typeStr = get_type_as_string(type);

    term_putc(terminal, '[');
    term_write(terminal, typeStr);
    term_write(terminal, "] ");
    term_write(terminal, label);
    term_write(terminal, ": 0x");
    term_write_hex(terminal, value);
    term_putc(terminal, '\n');
}

const char* get_type_as_string(LogType type) {
    switch (type) {
        case LOG_INFO:      return "INFO";
        case LOG_SUCCESS:   return "SUCCESS";
        case LOG_WARNING:   return "WARNING";
        case LOG_ERROR:     return "ERROR";
        case LOG_DEBUG:     return "DEBUG";
        case LOG_PANIC:     return "PANIC";
        default:            return "UNKNOWN";
    }
}