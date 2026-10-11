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

static void cursor_restore_background(void);
static Vector2u cursor_get_render_position(void);

void cursor_init(void) {
    cursor_position.x = (i32)(renderer_get_width() / 2);
    cursor_position.y = (i32)(renderer_get_height() / 2);

    cursor_present_overlay();
}

void cursor_handle_mouse(MouseEvent event) {
    Vector2u old_position = cursor_get_render_position();
    renderer_present_rect(old_position, cursor_size);

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

    cursor_present_overlay();
}

void cursor_draw(void) {
    renderer_fill_rect(
        cursor_get_render_position(), 
        cursor_size,
        cursor_color
    );
}

void cursor_present_overlay(void) {
    Vector2u position = cursor_get_render_position();
    renderer_read_rect(position, cursor_size, cursor_background);
    cursor_draw();
    
    renderer_present_rect(position, cursor_size);
    cursor_restore_background();
}

static void cursor_restore_background(void) {
    renderer_write_rect(cursor_get_render_position(), cursor_size, cursor_background);
}

static Vector2u cursor_get_render_position(void) {
    return (Vector2u){(u32)cursor_position.x, (u32)cursor_position.y};
}