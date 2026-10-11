#include <graphics/graphic_terminal.h>
#include <graphics/renderer.h>
#include <timer/timer.h>

void graphic_terminal_init(GraphicTerminal* terminal, const GraphicTerminalConfig* config) {
    if (terminal == NULL || config == NULL || config->font == NULL) {
        return;
    }

    terminal->origin = config->origin;
    terminal->size = config->size;
    terminal->font = config->font;
    terminal->scale = config->scale == 0 ? 1 : config->scale;
    terminal->line_spacing = config->line_spacing;
    terminal->foreground = config->foreground;
    terminal->background = config->background;
    terminal->cursor = (Vector2u){0, 0};
    terminal->cursor_visible = false;
    terminal->cursor_last_blink_tick = 0;
    terminal->scroll_offset = 0;
}

void graphic_terminal_putchar(GraphicTerminal* terminal, char character) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    u32 char_width = graphic_terminal_get_char_width(terminal);
    u32 char_height = graphic_terminal_get_char_height(terminal);
    u32 line_height = graphic_terminal_get_line_height(terminal);

    if (character == '\n') {
        terminal->cursor.x = 0;
        terminal->cursor.y += line_height;

        if (terminal->cursor.y + char_height > terminal->size.y) {
            graphic_terminal_scroll(terminal);
        }

        return;
    }

    if (terminal->cursor.x + char_width > terminal->size.x) {
        terminal->cursor.x = 0;
        terminal->cursor.y += line_height;

        if (terminal->cursor.y + char_height > terminal->size.y) {
            graphic_terminal_scroll(terminal);
        }
    }
    
    Vector2u position = vector2u_add(terminal->origin, terminal->cursor);
    font_draw_char(terminal->font, character, position, terminal->scale, terminal->foreground);
    terminal->cursor.x += char_width;
}

void graphic_terminal_draw_char_at(GraphicTerminal* terminal, char character, Vector2u position) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    Vector2u pixel_position = vector2u_add(terminal->origin, position);
    font_draw_char(terminal->font, character, pixel_position, terminal->scale, terminal->foreground);
}

void graphic_terminal_write(GraphicTerminal* terminal, const char* text) {
    if (terminal == NULL || text == NULL) {
        return;
    }

    for (size_t i = 0; text[i] != '\0'; i++) {
        graphic_terminal_putchar(terminal, text[i]);
    }
}

void graphic_terminal_backspace(GraphicTerminal* terminal) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    u32 char_width = graphic_terminal_get_char_width(terminal);
    u32 char_height = graphic_terminal_get_char_height(terminal);
    if (terminal->cursor.x >= char_width) {
        terminal->cursor.x -= char_width;

        Vector2u position = vector2u_add(terminal->origin, terminal->cursor);
        renderer_fill_rect(position, (Size2u){char_width, char_height}, terminal->background);
    }
}

void graphic_terminal_clear(GraphicTerminal* terminal) {
    if (terminal == NULL) {
        return;
    }

    renderer_fill_rect(terminal->origin, terminal->size, terminal->background);
    terminal->cursor = (Vector2u){0, 0};
}

void graphic_terminal_clear_chars(GraphicTerminal* terminal, u32 count) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    u32 char_width = graphic_terminal_get_char_width(terminal);
    if (char_width == 0) {
        return;
    }

    u32 char_height = graphic_terminal_get_char_height(terminal);
    if (char_height == 0) {
        return;
    }

    u32 line_height = graphic_terminal_get_line_height(terminal);
    if (line_height == 0) {
        return;
    }

    Vector2u position = terminal->cursor;
    for (u32 i = 0; i < count; i++) {
        if (position.x + char_width > terminal->size.x) {
            position.x = 0;
            position.y += line_height;
        }

        if (position.y + char_height > terminal->size.y) {
            break;
        }

        Vector2u pixel_position = {
            terminal->origin.x + position.x,
            terminal->origin.y + position.y
        };

        renderer_fill_rect(pixel_position, (Size2u){char_width, char_height}, terminal->background);
        position.x += char_width;
    }
}

void graphic_terminal_draw_cursor(GraphicTerminal* terminal) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    if (terminal->cursor_visible) {
        return;
    }

    u32 char_width = graphic_terminal_get_char_width(terminal);
    if (char_width > GRAPHIC_TERMINAL_CURSOR_MAX_WIDTH) {
        return;
    }

    u32 char_height = graphic_terminal_get_char_height(terminal);
    Vector2u cursor_position = {
        terminal->origin.x + terminal->cursor.x,
        (terminal->origin.y + terminal->cursor.y) + char_height - GRAPHIC_TERMINAL_CURSOR_HEIGHT
    };

    renderer_read_rect(cursor_position, (Size2u){char_width, GRAPHIC_TERMINAL_CURSOR_HEIGHT}, terminal->cursor_saved_pixels);
    renderer_fill_rect(cursor_position, (Size2u){char_width, GRAPHIC_TERMINAL_CURSOR_HEIGHT}, terminal->foreground);
    terminal->cursor_visible = true;
}

void graphic_terminal_hide_cursor(GraphicTerminal* terminal) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    if (!terminal->cursor_visible) {
        return;
    }

    u32 char_width = graphic_terminal_get_char_width(terminal);
    if (char_width > GRAPHIC_TERMINAL_CURSOR_MAX_WIDTH) {
        return;
    }

    u32 char_height = graphic_terminal_get_char_height(terminal);
    Vector2u cursor_position = {
        terminal->origin.x + terminal->cursor.x,
        (terminal->origin.y + terminal->cursor.y) + char_height - GRAPHIC_TERMINAL_CURSOR_HEIGHT
    };

    if (char_width == 0) {
        return;
    }

    renderer_write_rect(cursor_position, (Size2u){char_width, GRAPHIC_TERMINAL_CURSOR_HEIGHT}, terminal->cursor_saved_pixels);
    terminal->cursor_visible = false;
}

void graphic_terminal_toggle_cursor(GraphicTerminal* terminal) {
    if (terminal == NULL) {
        return;
    }

    if (terminal->cursor_visible) {
        graphic_terminal_hide_cursor(terminal);
    } else {
        graphic_terminal_draw_cursor(terminal);
    }
}

void graphic_terminal_present_cursor(GraphicTerminal* terminal) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    u32 char_width = graphic_terminal_get_char_width(terminal);
    if (char_width == 0) {
        return;
    }

    if (char_width > GRAPHIC_TERMINAL_CURSOR_MAX_WIDTH) {
        return;
    }

    u32 char_height = graphic_terminal_get_char_height(terminal);
    if (char_height < GRAPHIC_TERMINAL_CURSOR_HEIGHT) {
        return;
    }

    Vector2u cursor_position = {
        terminal->origin.x + terminal->cursor.x,
        (terminal->origin.y + terminal->cursor.y) + char_height - GRAPHIC_TERMINAL_CURSOR_HEIGHT
    };

    renderer_present_rect(cursor_position, (Size2u){char_width, GRAPHIC_TERMINAL_CURSOR_HEIGHT});
}

bool graphic_terminal_update_cursor(GraphicTerminal* terminal) {
    if (terminal == NULL) {
        return false;
    }
    
    u64 interval = timer_ticks_from_ms(GRAPHIC_TERMINAL_CURSOR_BLINK_MS);
    if (interval == 0) {
        return false;
    }

    u64 current_tick = timer_get_ticks();
    if (current_tick - terminal->cursor_last_blink_tick >= interval) {
        graphic_terminal_toggle_cursor(terminal);
        terminal->cursor_last_blink_tick = current_tick;
        return true;
    }

    return false;
}

void graphic_terminal_reset_cursor_blink(GraphicTerminal* terminal) {
    if (terminal == NULL) {
        return;
    }

    terminal->cursor_last_blink_tick = timer_get_ticks();
}

void graphic_terminal_set_cursor_column(GraphicTerminal* terminal, u32 column) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    u32 char_width = graphic_terminal_get_char_width(terminal);
    if (char_width == 0) {
        return;
    }

    u32 max_x = terminal->size.x - char_width;
    u32 x = column * char_width;
    if (x > max_x) {
        x = max_x;
    }

    terminal->cursor.x = x;
}

bool graphic_terminal_scroll(GraphicTerminal* terminal) {
    u32 char_height = graphic_terminal_get_char_height(terminal);
    u32 line_height = graphic_terminal_get_line_height(terminal);
    if (line_height == 0 || line_height > terminal->size.y) {
        return false;
    }

    Vector2u src = {
        terminal->origin.x,
        terminal->origin.y + line_height
    };

    Vector2u dest = terminal->origin;

    Size2u copy_size = {
        terminal->size.x,
        terminal->size.y - line_height
    };

    renderer_copy_rect(src, dest, copy_size);
    renderer_fill_rect(
        (Vector2u) {
            terminal->origin.x,
            terminal->origin.y + terminal->size.y - line_height
        },
        (Size2u) {
            terminal->size.x,
            line_height
        },
        terminal->background
    );

    if (terminal->cursor.y >= line_height) {
        terminal->cursor.y -= line_height;
    } else {
        terminal->cursor.y = 0;
    }

    terminal->scroll_offset += line_height;
    return true;
}

u32 graphic_terminal_get_char_width(const GraphicTerminal* terminal) {
    if (terminal == NULL) {
        return 0;
    }

    return terminal->font->glyph_size.x * terminal->scale;
}

u32 graphic_terminal_get_char_height(const GraphicTerminal* terminal) {
    if (terminal == NULL) {
        return 0;
    }

    return terminal->font->glyph_size.y * terminal->scale;
}

u32 graphic_terminal_get_line_height(const GraphicTerminal* terminal) {
    if (terminal == NULL) {
        return 0;
    }

    return graphic_terminal_get_char_height(terminal) + terminal->line_spacing;
}