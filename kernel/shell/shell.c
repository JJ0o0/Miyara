#include <shell/shell.h>
#include <keyboard/keymap.h>
#include <string/string.h>

typedef void (*ShellCommandHandler)(Shell* shell, int argc, const char** argv);

typedef struct {
    const char* name;
    const char* description;
    ShellCommandHandler handler;
} ShellCommand;

static void shell_print_prompt(Shell* shell);
static void shell_execute_command(Shell* shell);
static int  shell_parse_arguments(char* input, const char** argv);

static void shell_history_add(Shell* shell);
static void shell_history_previous(Shell* shell);
static void shell_history_next(Shell* shell);

static void shell_command_help(Shell* shell, int argc, const char** argv);
static void shell_command_clear(Shell* shell, int argc, const char** argv);
static void shell_command_echo(Shell* shell, int argc, const char** argv);
static void shell_command_version(Shell* shell, int argc, const char** argv);

static const ShellCommand commands[] = {
    {
        .name = "help",
        .description = "Lists available commands.",
        .handler = shell_command_help
    },
    {
        .name = "clear",
        .description = "Clears the terminal.",
        .handler = shell_command_clear
    },
    {
        .name = "echo",
        .description = "Prints text on the terminal.",
        .handler = shell_command_echo
    },
    {
        .name = "version",
        .description = "Shows the Miyara version.",
        .handler = shell_command_version
    },
};

void shell_init(Shell* shell, GraphicTerminal* terminal) {
    if (shell == NULL || terminal == NULL) {
        return;
    }

    shell->terminal = terminal;

    shell->input_length = 0;
    shell->input_buffer[0] = '\0';

    shell->history_count = 0;
    shell->history_index = 0;
    shell->history_draft[0] = '\0';

    shell->history_browsing = false;

    shell->cursor_index = 0;

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
            if (shell->cursor_index == 0) {
                return;
            }

            graphic_terminal_hide_cursor(shell->terminal);

            u32 delete_index = shell->cursor_index - 1;
            for (u32 i = delete_index; i < shell->input_length - 1; i++) {
                shell->input_buffer[i] = shell->input_buffer[i + 1];
            }

            shell->input_length--;
            shell->cursor_index--;

            shell->input_buffer[shell->input_length] = '\0';

            graphic_terminal_set_cursor_column(shell->terminal, SHELL_PROMPT_LENGTH + delete_index);
            graphic_terminal_clear_chars(shell->terminal, shell->input_length - delete_index + 1);
            graphic_terminal_write(shell->terminal, &shell->input_buffer[delete_index]);
            graphic_terminal_set_cursor_column(shell->terminal, SHELL_PROMPT_LENGTH + shell->cursor_index);

            graphic_terminal_draw_cursor(shell->terminal);
            return;
        case KEY_ENTER:
            graphic_terminal_hide_cursor(shell->terminal);
            graphic_terminal_putchar(shell->terminal, '\n');
            
            shell->input_buffer[shell->input_length] = '\0';
            shell_history_add(shell);
            shell_execute_command(shell);

            shell->input_buffer[0] = '\0';
            shell->input_length = 0;
            shell->history_browsing = false;
            shell->history_draft[0] = '\0';
            shell->cursor_index = 0;

            shell_print_prompt(shell);
            graphic_terminal_draw_cursor(shell->terminal);
            return;
        case KEY_UP:
            graphic_terminal_hide_cursor(shell->terminal);
            graphic_terminal_set_cursor_column(shell->terminal, SHELL_PROMPT_LENGTH + shell->input_length);

            shell_history_previous(shell);
            graphic_terminal_draw_cursor(shell->terminal);
            return;
        case KEY_DOWN:
            graphic_terminal_hide_cursor(shell->terminal);
            graphic_terminal_set_cursor_column(shell->terminal, SHELL_PROMPT_LENGTH + shell->input_length);

            shell_history_next(shell);
            graphic_terminal_draw_cursor(shell->terminal);
            return;
        case KEY_LEFT:
            if (shell->cursor_index > 0) {
                graphic_terminal_hide_cursor(shell->terminal);
                shell->cursor_index--;
            }

            graphic_terminal_set_cursor_column(shell->terminal, SHELL_PROMPT_LENGTH + shell->cursor_index);
            graphic_terminal_draw_cursor(shell->terminal);
            return;
        case KEY_RIGHT:
            if (shell->cursor_index < shell->input_length) {
                graphic_terminal_hide_cursor(shell->terminal);
                shell->cursor_index++;
            }

            graphic_terminal_set_cursor_column(shell->terminal, SHELL_PROMPT_LENGTH + shell->cursor_index);
            graphic_terminal_draw_cursor(shell->terminal);
            return;
        default:
            break;
    }

    char character;
    if (!keymap_translate(key_event, &character)) {
        return;
    }

    if (shell->input_length >= SHELL_MAXIMUM_INPUT_SIZE - 1) {
        return;
    }

    graphic_terminal_hide_cursor(shell->terminal);
    u32 insertion_index = shell->cursor_index;
    for (u32 i = shell->input_length; i > insertion_index; i--) {
        shell->input_buffer[i] = shell->input_buffer[i - 1];
    }

    shell->input_buffer[insertion_index] = character;

    shell->input_length++;
    shell->cursor_index++;

    shell->input_buffer[shell->input_length] = '\0';

    graphic_terminal_set_cursor_column(shell->terminal, SHELL_PROMPT_LENGTH + insertion_index);
    graphic_terminal_clear_chars(shell->terminal, shell->input_length - insertion_index);
    graphic_terminal_write(shell->terminal, &shell->input_buffer[insertion_index]);
    graphic_terminal_set_cursor_column(shell->terminal, SHELL_PROMPT_LENGTH + shell->cursor_index);

    graphic_terminal_draw_cursor(shell->terminal);
}

static void shell_print_prompt(Shell* shell) {
    graphic_terminal_write(shell->terminal, "Miyara> ");
}

static void shell_execute_command(Shell* shell) {
    if (shell == NULL) {
        return;
    }

    const char* argv[SHELL_MAXIMUM_ARGUMENTS];
    int argc = shell_parse_arguments(shell->input_buffer, argv);
    if (argc == 0) {
        return;
    }

    size_t commands_count = sizeof(commands) / sizeof(commands[0]);
    for (size_t i = 0; i < commands_count; i++) {
        if (!string_equals(argv[0], commands[i].name)) {
            continue;
        }

        commands[i].handler(shell, argc, argv);
        return;
    }

    graphic_terminal_write(shell->terminal, "Unknown command. Use 'help' to see available commands.\n");
}

static int shell_parse_arguments(char* input, const char** argv) {
    int argc = 0;
    char* current = input;

    while (*current != '\0') {
        while (*current == ' ') {
            current++;
        }

        if (*current == '\0') {
            break;
        }

        if (argc == SHELL_MAXIMUM_ARGUMENTS) {
            break;
        }

        argv[argc] = current;
        argc++;

        while (*current != ' ' && *current != '\0') {
            current++;
        }

        if (*current == ' ') {
            *current = '\0';
            current++;
        }
    }

    return argc; 
}

static void shell_history_add(Shell* shell) {
    if (shell->input_length == 0) {
        return;
    }

    if (shell->history_count < SHELL_HISTORY_SIZE) {
        string_copy(shell->input_buffer, shell->history[shell->history_count]);
        shell->history_count++;
    } else {
        for (size_t i = 1; i < SHELL_HISTORY_SIZE; i++) {
            string_copy(shell->history[i], shell->history[i - 1]);
        }

        string_copy(shell->input_buffer, shell->history[SHELL_HISTORY_SIZE - 1]);
    }

    shell->history_index = shell->history_count;
}

static void shell_history_previous(Shell* shell) {
    if (shell->history_count == 0) {
        return;
    }

    if (!shell->history_browsing) {
        string_copy(shell->input_buffer, shell->history_draft);
        shell->history_browsing = true;
        shell->history_index = shell->history_count;
    }

    if (shell->history_index > 0) {
        shell->history_index--;
    }

    while (shell->input_length > 0) {
        graphic_terminal_backspace(shell->terminal);
        shell->input_length--;
    }

    string_copy(shell->history[shell->history_index], shell->input_buffer);
    shell->input_length = string_len(shell->input_buffer);
    shell->cursor_index = shell->input_length;
    graphic_terminal_write(shell->terminal, shell->input_buffer);
}

static void shell_history_next(Shell* shell) {
    if (shell->history_count == 0) {
        return;
    }

    while (shell->input_length > 0) {
        graphic_terminal_backspace(shell->terminal);
        shell->input_length--;
    }

    if (shell->history_index < shell->history_count - 1) {
        shell->history_index++;

        string_copy(shell->history[shell->history_index], shell->input_buffer);
        shell->input_length = string_len(shell->input_buffer);
        shell->cursor_index = shell->input_length;
        graphic_terminal_write(shell->terminal, shell->input_buffer);
    } else {
        shell->history_index = shell->history_count;

        string_copy(shell->history_draft, shell->input_buffer);
        shell->input_length = string_len(shell->input_buffer);
        shell->cursor_index = shell->input_length;
        graphic_terminal_write(shell->terminal, shell->input_buffer);
        
        shell->history_browsing = false;
    }
}

static void shell_command_help(Shell* shell, int argc, const char** argv) {
    if (argc > 2) {
        graphic_terminal_write(shell->terminal, "Usage: help [command]\n");
        return;
    }

    if (argc == 1) {
        graphic_terminal_write(shell->terminal, "Available Commands:\n");
        graphic_terminal_write(shell->terminal, "===================\n");
    }

    size_t commands_count = sizeof(commands) / sizeof(commands[0]);
    for (size_t i = 0; i < commands_count; i++) {
        const ShellCommand* command = &commands[i];
        if (argc == 2) {
            if (!string_equals(argv[1], command->name)) {
                continue;
            }

            graphic_terminal_write(shell->terminal, command->name);
            graphic_terminal_write(shell->terminal, ": ");
            graphic_terminal_write(shell->terminal, command->description);
            graphic_terminal_putchar(shell->terminal, '\n');
            return;
        }

        graphic_terminal_write(shell->terminal, command->name);
        graphic_terminal_write(shell->terminal, ": ");
        graphic_terminal_write(shell->terminal, command->description);
        graphic_terminal_putchar(shell->terminal, '\n');
    }

    if (argc == 2) {
        graphic_terminal_write(shell->terminal, "Command not found.\n");
    }
}

static void shell_command_clear(Shell* shell, int argc, const char** argv) {
    if (argc != 1) {
        graphic_terminal_write(shell->terminal, "Usage: clear\n");
        return;
    }

    graphic_terminal_clear(shell->terminal);
}

static void shell_command_echo(Shell* shell, int argc, const char** argv) {
    if (argc == 1) {
        graphic_terminal_putchar(shell->terminal, '\n');
        return;
    }

    for (size_t i = 1; i < argc; i++) {
        graphic_terminal_write(shell->terminal, argv[i]);
        
        if (i + 1 < argc) {
            graphic_terminal_putchar(shell->terminal, ' ');
        }
    }

    graphic_terminal_putchar(shell->terminal, '\n');
}

static void shell_command_version(Shell* shell, int argc, const char** argv) {
    if (argc != 1) {
        graphic_terminal_write(shell->terminal, "Usage: version\n");
        return;
    }

    graphic_terminal_write(shell->terminal, "Miyara Development Build\n");
}