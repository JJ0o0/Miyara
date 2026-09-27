#ifndef LOG_H
#define LOG_H

#include <terminal/terminal.h>
#include <types/types.h>

/**
 * Severity/category of a log message.
 *
 * Printed as a tag ("[INFO]", "[ERROR]", etc.) before the message.
 */
typedef enum {
    LOG_INFO,
    LOG_SUCCESS,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG,
    LOG_PANIC
} LogType;

/**
 * Sets the terminal used by log() and log_hex().
 *
 * Must be called once, before any other logging function, since
 * the terminal is stored in a static and not passed per call.
 *
 * @param term Terminal to write log output to.
 */
void log_init(Terminal* term);

/**
 * Writes a tagged log message, followed by a newline.
 *
 * Format: "[TYPE] msg\n"
 *
 * @param type Severity/category of the message.
 * @param msg Null-terminated message to log.
 */
void log(LogType type, const char* msg);

/**
 * Writes a tagged log message with a labeled signed integer,
 * followed by a newline.
 *
 * Format: "[TYPE] label: value\n"
 *
 * @param type Severity/category of the message.
 * @param label Null-terminated label describing the value.
 * @param value Signed integer to write in decimal.
 */
void log_int(LogType type, const char* label, int value);

/**
 * Writes a tagged log message with a labeled boolean, followed by
 * a newline.
 *
 * Format: "[TYPE] label: True\n" or "[TYPE] label: False\n"
 *
 * @param type Severity/category of the message.
 * @param label Null-terminated label describing the value.
 * @param value Boolean to write ("True"/"False").
 */
void log_bool(LogType type, const char* label, bool value);

/**
 * Writes a tagged log message with a labeled hexadecimal value,
 * followed by a newline.
 *
 * Format: "[TYPE] label: 0xVALUE\n"
 *
 * @param type Severity/category of the message.
 * @param label Null-terminated label describing the value.
 * @param value Value to write in hexadecimal.
 */
void log_hex(LogType type, const char* label, u64 value);

#endif