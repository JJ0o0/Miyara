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
    terminal->foreground = config->foreground;
    terminal->background = config->background;
    terminal->cursor = (Vector2u){0, 0};
}

void graphic_terminal_putchar(GraphicTerminal* terminal, char character) {
    if (terminal == NULL || terminal->font == NULL) {
        return;
    }

    u32 char_width = terminal->font->glyph_size.x * terminal->scale;
    u32 char_height = terminal->font->glyph_size.y * terminal->scale;

    if (character == '\n') {
        terminal->cursor.x = 0;
        terminal->cursor.y += char_height;

        if (terminal->cursor.y + char_height > terminal->size.y) {
            graphic_terminal_scroll(terminal);
        }

        return;
    }

    if (terminal->cursor.x + char_width > terminal->size.x) {
        terminal->cursor.x = 0;
        terminal->cursor.y += char_height;

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
    if (terminal->cursor.x > 0) {
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

static void graphic_terminal_scroll(GraphicTerminal* terminal) {
    u32 line_height = terminal->font->glyph_size.y * terminal->scale;
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