#include <shell/shell.h>
#include <shell/commands.h>

#include <keyboard/keymap.h>
#include <string/string.h>
#include <math/math.h>

static void shell_print_prompt(Shell* shell);
static void shell_submit(Shell* shell);
static int  shell_parse_arguments(char* input, const char** argv);

static void shell_handle_backspace(Shell* shell);
static void shell_handle_delete(Shell* shell);
static void shell_handle_enter(Shell* shell);
static void shell_move_cursor(Shell* shell, u32 index);
static void shell_insert_character(Shell* shell, char character);
static void shell_redraw_from(Shell* shell, u32 index, u32 clear_count);
static bool shell_set_terminal_cursor(Shell* shell, u32 index);

static void shell_replace_input(Shell* shell, const char* text);
static void shell_history_add(Shell* shell);
static void shell_history_previous(Shell* shell);
static void shell_history_next(Shell* shell);

static Vector2i shell_get_input_position(Shell* shell, u32 index);

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
    shell->input_scroll_offset = 0;

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
            shell_handle_backspace(shell);
            return;
        case KEY_DELETE:
            shell_handle_delete(shell);
            return;
        case KEY_ENTER:
            shell_handle_enter(shell);
            return;
        case KEY_UP:
            shell_history_previous(shell);
            return;
        case KEY_DOWN:
            shell_history_next(shell);
            return;
        case KEY_LEFT:
            if (shell->cursor_index > 0) {
                shell_move_cursor(shell, shell->cursor_index - 1);
            }
            return;
        case KEY_RIGHT:
            if (shell->cursor_index < shell->input_length) {
                shell_move_cursor(shell, shell->cursor_index + 1);
            }
            return;
        case KEY_HOME:
            shell_move_cursor(shell, 0);
            return;
        case KEY_END:
            shell_move_cursor(shell, shell->input_length);
            return;
        default:
            break;
    }

    char character;
    if (!keymap_translate(key_event, &character)) {
        return;
    }

    shell_insert_character(shell, character);
}

static void shell_print_prompt(Shell* shell) {
    graphic_terminal_write(shell->terminal, "Miyara> ");
    shell->input_origin = shell->terminal->cursor;
    shell->input_scroll_offset = shell->terminal->scroll_offset;
}

static void shell_submit(Shell* shell) {
    const char* argv[SHELL_MAXIMUM_ARGUMENTS];
    int argc = shell_parse_arguments(shell->input_buffer, argv);

    shell_execute_command(shell, argc, argv);
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

static void shell_handle_backspace(Shell* shell) {
    if (shell == NULL) {
        return;
    }

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

    shell_redraw_from(shell, delete_index, shell->input_length - delete_index + 1);
}

static void shell_handle_delete(Shell* shell) {
    if (shell == NULL) {
        return;
    }

    if (shell->cursor_index >= shell->input_length) {
        return;
    }

    graphic_terminal_hide_cursor(shell->terminal);

    u32 delete_index = shell->cursor_index;
    for (u32 i = delete_index; i < shell->input_length - 1; i++) {
        shell->input_buffer[i] = shell->input_buffer[i + 1];
    }

    shell->input_length--;
    shell->input_buffer[shell->input_length] = '\0';

    shell_redraw_from(shell, delete_index, shell->input_length - delete_index + 1);
}

static void shell_handle_enter(Shell* shell) {
    if (shell == NULL) {
        return;
    }

    graphic_terminal_hide_cursor(shell->terminal);
    graphic_terminal_putchar(shell->terminal, '\n');
    
    shell->input_buffer[shell->input_length] = '\0';
    shell_history_add(shell);
    shell_submit(shell);

    shell->input_buffer[0] = '\0';
    shell->input_length = 0;
    shell->history_browsing = false;
    shell->history_draft[0] = '\0';
    shell->cursor_index = 0;

    shell_print_prompt(shell);
    graphic_terminal_reset_cursor_blink(shell->terminal);
    graphic_terminal_draw_cursor(shell->terminal);
}

static void shell_move_cursor(Shell* shell, u32 index) {
    if (shell == NULL) {
        return;
    }

    if (index > shell->input_length) {
        index = shell->input_length;
    }

    graphic_terminal_hide_cursor(shell->terminal);
    
    shell->cursor_index = index;
    
    if (shell_set_terminal_cursor(shell, shell->cursor_index)) {
        graphic_terminal_reset_cursor_blink(shell->terminal);
        graphic_terminal_draw_cursor(shell->terminal);
    }
}

static void shell_insert_character(Shell* shell, char character) {
    if (shell == NULL || character == '\0') {
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

    shell_redraw_from(shell, insertion_index, shell->input_length - insertion_index);
}

static void shell_redraw_from(Shell* shell, u32 index, u32 clear_count) {
    if (shell == NULL) {
        return;
    }

    u32 draw_index = index;
    while (draw_index < shell->input_length) {
        if (shell_set_terminal_cursor(shell, draw_index)) {
            break;
        }

        draw_index++;
    }

    if (draw_index < shell->input_length) {
        u32 skipped = draw_index - index;
        u32 visible_clear_count = 0;
        if (skipped < clear_count) {
            visible_clear_count = clear_count - skipped;
        }

        graphic_terminal_clear_chars(shell->terminal, visible_clear_count);
        
        for (u32 i = draw_index; i < shell->input_length; i++) {
            Vector2i position = shell_get_input_position(shell, i);
            if (position.x < 0 || position.y < 0) {
                continue;
            }

            if (position.y >= (i32)shell->terminal->size.y) {
                break;
            }

            graphic_terminal_draw_char_at(
                shell->terminal, shell->input_buffer[i], 
                (Vector2u){
                    (u32)position.x,
                    (u32)position.y
                }
            );
        }
    }

    if (shell_set_terminal_cursor(shell, shell->cursor_index)) {
        graphic_terminal_reset_cursor_blink(shell->terminal);
        graphic_terminal_draw_cursor(shell->terminal);
    }
}

static bool shell_set_terminal_cursor(Shell* shell, u32 index) {
    if (shell == NULL) {
        return false;
    }

    Vector2i position = shell_get_input_position(shell, index);
    if (position.x < 0 || position.y < 0) {
        return false;
    }

    if (position.x >= shell->terminal->size.x) {
        return false;
    }

    if (position.y >= shell->terminal->size.y) {
        return false;
    }

    shell->terminal->cursor = (Vector2u){
        (u32)position.x,
        (u32)position.y
    };

    return true;
}

static void shell_replace_input(Shell* shell, const char* text) {
    if (shell == NULL || text == NULL) {
        return;
    }

    u32 text_length = (u32)string_len(text);
    if (text_length >= SHELL_MAXIMUM_INPUT_SIZE) {
        return;
    }

    graphic_terminal_hide_cursor(shell->terminal);

    u32 old_length = shell->input_length;
    u32 old_draw_index = 0;
    while (old_draw_index < old_length) {
        if (shell_set_terminal_cursor(shell, old_draw_index)) {
            break;
        }

        old_draw_index++;
    }

    if (old_draw_index < old_length) {
        graphic_terminal_clear_chars(shell->terminal, old_length - old_draw_index);
    }

    string_copy(text, shell->input_buffer);

    shell->input_length = text_length;
    shell->cursor_index = text_length;

    u32 draw_index = 0;
    while (draw_index < shell->input_length) {
        if (shell_set_terminal_cursor(shell, draw_index)) {
            break;
        }

        draw_index++;
    }

    if (draw_index < shell->input_length) {
        graphic_terminal_write(shell->terminal, &shell->input_buffer[draw_index]);
    }

    if (shell_set_terminal_cursor(shell, shell->cursor_index)) {
        graphic_terminal_reset_cursor_blink(shell->terminal);
        graphic_terminal_draw_cursor(shell->terminal);
    }
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

    shell_replace_input(shell, shell->history[shell->history_index]);
}

static void shell_history_next(Shell* shell) {
    if (shell->history_count == 0 || !shell->history_browsing) {
        return;
    }

    if (shell->history_index < shell->history_count - 1) {
        shell->history_index++;

        shell_replace_input(shell, shell->history[shell->history_index]);
    } else {
        shell->history_index = shell->history_count;
        shell_replace_input(shell, shell->history_draft);
        shell->history_browsing = false;
    }
}

static Vector2i shell_get_input_position(Shell* shell, u32 index) {
    if (shell == NULL) {
        return (Vector2i){0, 0};
    }   

    u32 char_width = graphic_terminal_get_char_width(shell->terminal);
    if (char_width == 0) {
        return (Vector2i){0, 0};
    }

    u32 line_height = graphic_terminal_get_line_height(shell->terminal);
    if (line_height == 0) {
        return (Vector2i){0, 0};
    }

    u32 columns = shell->terminal->size.x / char_width;
    if (columns == 0) {
        return (Vector2i){0, 0};
    }

    u32 start_column = shell->input_origin.x / char_width;
    u32 cell = start_column + index;

    u32 column = cell % columns;
    u32 row = cell / columns;

    u64 scroll_delta = shell->terminal->scroll_offset - shell->input_scroll_offset;
    i64 y = (i64)shell->input_origin.y + ((i64)row * (i64)line_height) - (i64)scroll_delta;
    return (Vector2i){
        (i32)(column * char_width),
        (i32)y
    };
}