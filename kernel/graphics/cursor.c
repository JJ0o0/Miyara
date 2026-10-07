#include <graphics/cursor.h>
#include <graphics/renderer.h>
#include <math/math.h>
#include <math/vector.h>

#define CURSOR_WIDTH 8
#define CURSOR_HEIGHT 8

static Vector2i cursor_position;

static const Size2u cursor_size = { CURSOR_WIDTH, CURSOR_HEIGHT };
static const Color cursor_color = { 255, 250, 160 };

static u32 cursor_background[CURSOR_WIDTH * CURSOR_HEIGHT];

static Vector2u cursor_get_render_position(void);

void cursor_init(void) {
    cursor_position.x = (i32)(renderer_get_width() / 2);
    cursor_position.y = (i32)(renderer_get_height() / 2);

    Vector2u position = cursor_get_render_position();
    renderer_read_rect(position, cursor_size, cursor_background);
        cursor_draw();
    renderer_present_rect(position, cursor_size);
}

void cursor_handle_mouse(MouseEvent event) {
    Vector2u position = cursor_get_render_position();
    renderer_write_rect(position, cursor_size, cursor_background);
    renderer_present_rect(position, cursor_size);

    cursor_position.x += event.dx;
    cursor_position.y -= event.dy;

    cursor_position.x = clamp_i32(
        cursor_position.x,
        0,
        (i32)renderer_get_width() - CURSOR_WIDTH
    );

    cursor_position.y = clamp_i32(
        cursor_position.y,
        0,
        (i32)renderer_get_height() - CURSOR_HEIGHT
    );

    position = cursor_get_render_position();
    renderer_read_rect(position, cursor_size, cursor_background);
        cursor_draw();
    renderer_present_rect(position, cursor_size);
}

void cursor_draw(void) {
    renderer_fill_rect(
        cursor_get_render_position(), 
        cursor_size,
        cursor_color
    );
}

static Vector2u cursor_get_render_position(void) {
    return (Vector2u){(u32)cursor_position.x, (u32)cursor_position.y};
}