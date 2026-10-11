#include <shell/commands.h>
#include <shell/shell.h>
#include <string/string.h>
#include <graphics/graphic_terminal.h>

static ShellCommandRegistry registry;

static void shell_command_help(Shell* shell, int argc, const char** argv);
static void shell_command_clear(Shell* shell, int argc, const char** argv);
static void shell_command_echo(Shell* shell, int argc, const char** argv);
static void shell_command_version(Shell* shell, int argc, const char** argv);

void shell_commands_init(void) {
    registry.count = 0;

    shell_add_command(&(ShellCommand){
        .name = "help",
        .description = "Lists available commands.",
        .handler = shell_command_help
    });

    shell_add_command(&(ShellCommand){
        .name = "clear",
        .description = "Clears the terminal.",
        .handler = shell_command_clear
    });

    shell_add_command(&(ShellCommand){
        .name = "echo",
        .description = "Prints text on the terminal.",
        .handler = shell_command_echo
    });

    shell_add_command(&(ShellCommand){
        .name = "version",
        .description = "Shows the Miyara version.",
        .handler = shell_command_version
    });
}

bool shell_add_command(const ShellCommand* command) {
    if (command == NULL || command->name == NULL || command->handler == NULL) {
        return false;
    }

    if (registry.count >= SHELL_MAX_COMMANDS) {
        return false;
    }

    if (shell_find_command(command->name) != NULL) {
        return false;
    }

    registry.commands[registry.count] = *command;
    registry.count++;
    return true;
}

const ShellCommand* shell_find_command(const char* name) {
    if (name == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < registry.count; i++) {
        if (string_equals(name, registry.commands[i].name)) {
            return &registry.commands[i];
        }
    }

    return NULL;
}

void shell_execute_command(Shell* shell, int argc, const char** argv) {
    if (shell == NULL) {
        return;
    }

    if (argc == 0) {
        return;
    }

    const ShellCommand* command = shell_find_command(argv[0]);
    if (command == NULL) {
        graphic_terminal_write(shell->terminal, "Unknown command. Use 'help' to see available commands.\n");
        return;
    }

    command->handler(shell, argc, argv);
}

static void shell_command_help(Shell* shell, int argc, const char** argv) {
    if (argc > 2) {
        graphic_terminal_write(shell->terminal, "Usage: help [command]\n");
        return;
    }

    if (argc == 2) {
        const ShellCommand* command = shell_find_command(argv[1]);
        if (command == NULL) {
            graphic_terminal_write(shell->terminal, "Command not found.\n");
            return;
        }

        graphic_terminal_write(shell->terminal, command->name);
        graphic_terminal_write(shell->terminal, ": ");
        graphic_terminal_write(shell->terminal, command->description);
        graphic_terminal_putchar(shell->terminal, '\n');
        return;
    }

    graphic_terminal_write(shell->terminal, "Available Commands:\n");
    graphic_terminal_write(shell->terminal, "===================\n");
    for (size_t i = 0; i < registry.count; i++) {
        const ShellCommand* command = &registry.commands[i];
        graphic_terminal_write(shell->terminal, command->name);
        graphic_terminal_write(shell->terminal, ": ");
        graphic_terminal_write(shell->terminal, command->description);
        graphic_terminal_putchar(shell->terminal, '\n');
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