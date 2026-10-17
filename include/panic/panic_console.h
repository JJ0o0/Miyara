#ifndef PANIC_CONSOLE_H
#define PANIC_CONSOLE_H

#include <types/types.h>

void panic_console_clear(void);
void panic_console_write(const char* text);
void panic_console_write_hex(const char* label, u64 value);

#endif