#include <graphics/cursor.h>
#include <graphics/framebuffer.h>
#include <math/math.h>

#define CURSOR_WIDTH 8
#define CURSOR_HEIGHT 8

static i32 cursor_x;
static i32 cursor_y;

void cursor_init(void) {
    cursor_x = (i32)(framebuffer_get_width() / 2);
    cursor_y = (i32)(framebuffer_get_height() / 2);
}

void cursor_handle_mouse(MouseEvent event) {
    framebuffer_fill_rect(
        cursor_x, cursor_y,
        CURSOR_WIDTH, CURSOR_HEIGHT,
        (Color){0, 0, 0}
    );

    cursor_x += event.dx;
    cursor_y -= event.dy;

    cursor_x = clamp_i32(
        cursor_x,
        0,
        (i32)framebuffer_get_width() - CURSOR_WIDTH
    );

    cursor_y = clamp_i32(
        cursor_y,
        0,
        (i32)framebuffer_get_height() - CURSOR_HEIGHT
    );

    cursor_draw();
}

void cursor_draw(void) {
    framebuffer_fill_rect(
        cursor_x, cursor_y,
        CURSOR_WIDTH, CURSOR_HEIGHT,
        (Color){93, 84, 77}
    );
}