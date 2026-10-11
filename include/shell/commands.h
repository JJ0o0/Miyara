#ifndef COMMANDS_H
#define COMMANDS_H

#include <types/types.h>

/**
 * Maximum number of commands that can be registered at the same time.
 */
#define SHELL_MAX_COMMANDS 32

/**
 * Forward declaration of the Shell type.
 *
 * This avoids requiring commands.h to include shell.h directly.
 */
typedef struct Shell Shell;

/**
 * Function signature used by shell command handlers.
 *
 * @param shell Shell instance that invoked the command.
 * @param argc Number of arguments, including the command name.
 * @param argv Array of NUL-terminated argument strings.
 */
typedef void (*ShellCommandHandler)(
    Shell* shell,
    int argc,
    const char** argv
);

/**
 * Describes a shell command.
 */
typedef struct {
    /**
     * Command name used to invoke it from the shell.
     */
    const char* name;

    /**
     * Short human-readable description of the command.
     */
    const char* description;

    /**
     * Function called when the command is executed.
     */
    ShellCommandHandler handler;
} ShellCommand;

/**
 * Registry containing all currently registered shell commands.
 *
 * Commands are stored directly in a fixed-size array. The registry does
 * not allocate memory dynamically.
 */
typedef struct {
    /**
     * Registered commands.
     */
    ShellCommand commands[SHELL_MAX_COMMANDS];

    /**
     * Number of valid commands currently stored in the registry.
     */
    u32 count;
} ShellCommandRegistry;

/**
 * Initializes the shell command registry.
 *
 * Removes all previously registered commands by resetting the registry
 * count to zero.
 */
void shell_commands_init(void);

/**
 * Adds a command to the shell command registry.
 *
 * The command structure is copied into the registry. The command name
 * and description pointers themselves are not duplicated, so the
 * referenced strings must remain valid while the command is registered.
 *
 * Registration fails if the command is invalid, the registry is full,
 * or another command with the same name already exists.
 *
 * @param command Command to register.
 *
 * @return true if the command was registered successfully.
 * @return false if the command could not be registered.
 */
bool shell_add_command(const ShellCommand* command);

/**
 * Searches the registry for a command by name.
 *
 * @param name NUL-terminated command name to search for.
 *
 * @return Pointer to the registered command if found.
 * @return NULL if no command with that name exists or name is NULL.
 */
const ShellCommand* shell_find_command(const char* name);

/**
 * Executes a command parsed from shell input.
 *
 * The command name is expected in argv[0]. The function searches the
 * registry for the matching command and invokes its handler.
 *
 * If no matching command exists, an error message is written to the
 * shell terminal.
 *
 * @param shell Shell instance requesting command execution.
 * @param argc Number of parsed arguments.
 * @param argv Array of parsed argument strings.
 */
void shell_execute_command(Shell* shell, int argc, const char** argv);

#endif