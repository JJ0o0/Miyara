#ifndef SHELL_H
#define SHELL_H

#include <graphics/graphic_terminal.h>
#include <keyboard/keyboard.h>
#include <types/types.h>

/**
 * Size of the shell's input buffer, in bytes, including the
 * terminating NUL. A command can therefore have at most
 * SHELL_MAXIMUM_INPUT_SIZE - 1 (255) characters.
 */
#define SHELL_MAXIMUM_INPUT_SIZE 256

#define SHELL_MAXIMUM_ARGUMENTS  16

/**
 * State of a shell: the line being typed and the terminal it talks to.
 */
typedef struct {
    /**
     * Terminal where the prompt, the typed characters and the command
     * output are written. Not owned by the shell.
     */
    GraphicTerminal* terminal;

    /**
     * The line being typed, always NUL-terminated.
     */
    char input_buffer[SHELL_MAXIMUM_INPUT_SIZE];

    /**
     * Number of characters in input_buffer, not counting the NUL.
     */
    u32 input_length;
} Shell;

/**
 * Initializes a shell on a terminal and prints the first prompt
 * ("Miyara> ").
 *
 * The terminal must already be initialized. Does nothing if shell or
 * terminal is NULL.
 *
 * @param shell Shell to initialize.
 * @param terminal Terminal the shell reads output from and writes to.
 */
void shell_init(Shell* shell, GraphicTerminal* terminal);

/**
 * Feeds one key event to the shell.
 *
 * Key releases are ignored. Backspace erases the last typed character
 * (the prompt itself cannot be erased). Enter ends the line: it
 * echoes a newline, runs the command and prints a new prompt. Any
 * other key that keymap_translate() turns into a character is added
 * to the line and echoed, while there is room (up to
 * SHELL_MAXIMUM_INPUT_SIZE - 1 characters); keys without a character
 * are ignored.
 *
 * Only "help" is recognized as a command so far; any other command
 * prints an error message.
 *
 * @param shell Shell that receives the key. Ignored if NULL.
 * @param key_event Key press or release to handle.
 */
void shell_handle_key(Shell* shell, KeyEvent key_event);

#endif