#include <shell/shell.h>

#include <keyboard/keymap.h>

#include <string/string.h>

static void shell_print_prompt(Shell* shell);
static void shell_execute_command(Shell* shell);

void shell_init(Shell* shell, GraphicTerminal* terminal) {
    if (shell == NULL || terminal == NULL) {
        return;
    }

    shell->terminal = terminal;
    shell->input_length = 0;
    shell->input_buffer[0] = '\0';

    shell_print_prompt(shell);
}

void shell_handle_key(Shell* shell, KeyEvent key_event) {
    if (shell == NULL) {
        return;
    }

    if (key_event.state == KEY_RELEASED) {
        return;
    }

    switch (key_event.key) {
        case KEY_BACKSPACE:
            if (shell->input_length > 0) {
                shell->input_length--;
                shell->input_buffer[shell->input_length] = '\0';
                graphic_terminal_backspace(shell->terminal);
            }

            return;
        case KEY_ENTER:
            graphic_terminal_putchar(shell->terminal, '\n');
            
            shell->input_buffer[shell->input_length] = '\0';

            if (shell->input_buffer > 0) { 
                shell_execute_command(shell);
            }

            shell->input_length = 0;
            shell->input_buffer[0] = '\0';

            shell_print_prompt(shell);

            return;
    }

    char character;
    if (!keymap_translate(key_event, &character)) {
        return;
    }

    if (shell->input_length >= SHELL_MAXIMUM_INPUT_SIZE - 1) {
        return;
    }

    shell->input_buffer[shell->input_length] = character;
    shell->input_length++;

    shell->input_buffer[shell->input_length] = '\0';

    graphic_terminal_putchar(shell->terminal, character);
}

static void shell_print_prompt(Shell* shell) {
    graphic_terminal_write(shell->terminal, "Miyara> ");
}

static void shell_execute_command(Shell* shell) {
    if (shell == NULL) {
        return;
    }

    if (string_equals(shell->input_buffer, "help")) {
        graphic_terminal_write(shell->terminal, "It works!\n");
    } else {
        graphic_terminal_write(shell->terminal, "Invalid Command! Use 'help' to see available commands.\n");
    }
}