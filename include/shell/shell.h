#ifndef SHELL_H
#define SHELL_H

#include <graphics/graphic_terminal.h>
#include <keyboard/keyboard.h>
#include <types/types.h>

#define SHELL_MAXIMUM_INPUT_SIZE 256

typedef struct {
    GraphicTerminal* terminal;
    char input_buffer[SHELL_MAXIMUM_INPUT_SIZE];
    u32 input_length;
} Shell;

void shell_init(Shell* shell, GraphicTerminal* terminal);
void shell_handle_key(Shell* shell, KeyEvent key_event);

#endif