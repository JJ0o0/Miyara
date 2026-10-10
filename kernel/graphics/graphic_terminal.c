#include <graphics/graphic_terminal.h>
#include <graphics/renderer.h>

static void graphic_terminal_scroll(GraphicTerminal* terminal);

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
    terminal->cursor_blink_counter = 0;
}

void graphic_terminal_putchar(GraphicTerminal* terminal, char character) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    u32 char_width = terminal->font->glyph_size.x * terminal->scale;
    u32 char_height = terminal->font->glyph_size.y * terminal->scale;
    u32 line_height = char_height + terminal->line_spacing;

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

    u32 char_width = terminal->font->glyph_size.x * terminal->scale;
    u32 char_height = terminal->font->glyph_size.y * terminal->scale;
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

    u32 char_width = terminal->font->glyph_size.x * terminal->scale;
    u32 char_height = terminal->font->glyph_size.y * terminal->scale;
    Vector2u position = vector2u_add(terminal->origin, terminal->cursor);
    renderer_fill_rect(position, (Size2u){ char_width * count, char_height}, terminal->background);
}

void graphic_terminal_draw_cursor(GraphicTerminal* terminal) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    if (terminal->cursor_visible) {
        return;
    }

    u32 char_width = terminal->font->glyph_size.x * terminal->scale;
    u32 char_height = terminal->font->glyph_size.y * terminal->scale;

    u32 cursor_x = terminal->origin.x + terminal->cursor.x;
    u32 cursor_y = (terminal->origin.y + terminal->cursor.y) + char_height - 2;

    renderer_fill_rect((Vector2u){cursor_x, cursor_y}, (Size2u){char_width, 2}, terminal->foreground);
    terminal->cursor_visible = true;
}

void graphic_terminal_hide_cursor(GraphicTerminal* terminal) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    if (!terminal->cursor_visible) {
        return;
    }

    u32 char_width = terminal->font->glyph_size.x * terminal->scale;
    u32 char_height = terminal->font->glyph_size.y * terminal->scale;

    u32 cursor_x = terminal->origin.x + terminal->cursor.x;
    u32 cursor_y = (terminal->origin.y + terminal->cursor.y) + char_height - 2;
    renderer_fill_rect((Vector2u){cursor_x, cursor_y}, (Size2u){char_width, 2}, terminal->background);
    
    terminal->cursor_visible = false;
}

void graphic_terminal_toggle_cursor(GraphicTerminal* terminal) {
    if (terminal->cursor_visible) {
        graphic_terminal_hide_cursor(terminal);
    } else {
        graphic_terminal_draw_cursor(terminal);
    }
}

void graphic_terminal_set_cursor_column(GraphicTerminal* terminal, u32 column) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    u32 char_width = terminal->font->glyph_size.x * terminal->scale;
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

static void graphic_terminal_scroll(GraphicTerminal* terminal) {
    u32 char_height = terminal->font->glyph_size.y * terminal->scale;
    u32 line_height = char_height + terminal->line_spacing;
    if (line_height == 0 || line_height > terminal->size.y) {
        return;
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

    terminal->cursor.y -= line_height;
}