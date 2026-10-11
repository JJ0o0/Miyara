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

/**
 * Maximum number of words in a command line, counting the command name
 * and its arguments. Extra words are ignored.
 */
#define SHELL_MAXIMUM_ARGUMENTS  16

/**
 * Number of commands kept in the history. When it is full, the oldest
 * one is dropped to make room for the newest.
 */
#define SHELL_HISTORY_SIZE       16

/**
 * Length of the prompt ("Miyara> "), in characters. It is used to find
 * the terminal column of each position in the line, so it must match
 * the prompt the shell prints.
 */
#define SHELL_PROMPT_LENGTH      8

/**
 * State of a shell: the line being typed and its cursor, the command
 * history, and the terminal it talks to.
 */
typedef struct Shell {
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

    /**
     * Previous commands, oldest first. Only the first history_count
     * entries are valid.
     */
    char history[SHELL_HISTORY_SIZE][SHELL_MAXIMUM_INPUT_SIZE];

    /**
     * Number of commands stored in history, at most
     * SHELL_HISTORY_SIZE.
     */
    u32 history_count;

    /**
     * History entry being shown while browsing; equal to history_count
     * when no entry is selected.
     */
    u32 history_index;

    /**
     * The line that was being typed when browsing the history started.
     * It is restored when moving past the newest entry.
     */
    char history_draft[SHELL_MAXIMUM_INPUT_SIZE];

    /**
     * True while a history entry is shown instead of the line the user
     * was typing.
     */
    bool history_browsing;

    /**
     * Position of the editing cursor inside input_buffer: 0 is before
     * the first character and input_length is after the last one.
     */
    u32 cursor_index;
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
 * Key releases are ignored. The line is edited at the cursor:
 *  - Any key that keymap_translate() turns into a character is
 *    inserted at the cursor and echoed, while there is room (up to
 *    SHELL_MAXIMUM_INPUT_SIZE - 1 characters).
 *  - Backspace deletes the character before the cursor. The prompt
 *    itself cannot be erased.
 *  - Left, Right, Home and End move the cursor.
 *  - Up and Down browse the command history. The first Up saves the
 *    line being typed, and going Down past the newest entry restores
 *    it. They do nothing while the history is empty, and Down does
 *    nothing when the history is not being browsed.
 *  - Enter (or the keypad Enter) ends the line: it echoes a newline,
 *    adds the line to the history (unless it is empty), runs the
 *    command and prints a new prompt.
 * Other keys are ignored.
 *
 * A command line is split into words at spaces, up to
 * SHELL_MAXIMUM_ARGUMENTS. The available commands are help [command],
 * clear, echo [text...] and version. An empty line does nothing, and
 * an unknown command prints an error message.
 *
 * @param shell Shell that receives the key. Ignored if NULL.
 * @param key_event Key press or release to handle.
 */
void shell_handle_key(Shell* shell, KeyEvent key_event);

#endif