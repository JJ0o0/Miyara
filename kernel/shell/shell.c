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
        .description = "Prints message on the terminal.",
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

            shell_execute_command(shell);

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
        graphic_terminal_write(shell->terminal, "Usage: clear\n");
        return;
    }

    graphic_terminal_write(shell->terminal, "Miyara Development Build\n");
}