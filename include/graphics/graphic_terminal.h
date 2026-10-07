#ifndef GRAPHIC_TERMINAL_H
#define GRAPHIC_TERMINAL_H

#include <graphics/color.h>
#include <graphics/font.h>
#include <math/vector.h>
#include <types/types.h>

typedef struct {
    Vector2u origin;
    Size2u size;

    Vector2u cursor;

    const BitmapFont* font;
    u8 scale;

    Color foreground;
    Color background;
} GraphicTerminal;

typedef struct {
    Vector2u origin;
    Size2u size;

    const BitmapFont* font;
    u8 scale;

    Color foreground;
    Color background;
} GraphicTerminalConfig;

void graphic_terminal_init(GraphicTerminal* terminal, const GraphicTerminalConfig* config);
void graphic_terminal_putchar(GraphicTerminal* terminal, char character);
void graphic_terminal_write(GraphicTerminal* terminal, const char* text);
void graphic_terminal_clear(GraphicTerminal* terminal);

#endif