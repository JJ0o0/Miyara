#ifndef PANIC_H
#define PANIC_H

#include <cpu/context.h>

void panic(const char* message);
void panic_context(const char* message, CPUContext* context);

#endif