#include <graphics/renderer.h>
#include <graphics/framebuffer.h>

#include <memory/heap.h>

static u32* backbuffer = NULL;
static u32  renderer_width;
static u32  renderer_height;

bool renderer_init(void) {
    renderer_width = framebuffer_get_width();
    renderer_height = framebuffer_get_height();

    u64 pixel_count = (u64)renderer_width * renderer_height;
    u64 buffer_size = pixel_count * sizeof(u32);

    backbuffer = k_malloc(buffer_size);
    if (backbuffer == NULL) {
        return false;
    }

    return true;
}

u32 renderer_get_width(void) {
    return renderer_width;
}

u32 renderer_get_height(void) {
    return renderer_height;
}

void renderer_put_pixel(Vector2u position, Color color) {
    if (position.x >= renderer_width || position.y >= renderer_height) {
        return;
    }

    u64 index = (u64)position.y * renderer_width + position.x;
    backbuffer[index] = framebuffer_encode_color(color);
}

void renderer_clear(Color color) {
    u32 encoded_color = framebuffer_encode_color(color);
    u64 pixel_count = (u64)renderer_width * renderer_height;

    for (u64 i = 0; i < pixel_count; i++) {
        backbuffer[i] = encoded_color;
    }
}

void renderer_fill_rect(Vector2u position, Size2u size, Color color) {
    for (u32 py = 0; py < size.y; py++) {
        for (u32 px = 0; px < size.x; px++) {
            renderer_put_pixel(
                (Vector2u){
                    position.x + px,
                    position.y + py
                },
                color
            );
        }
    }
}

void renderer_read_rect(Vector2u position, Size2u size, u32* pixels) {
    if (pixels == NULL) {
        return;
    }

    if (position.x >= renderer_width || position.y >= renderer_height) {
        return;
    }

    u32 buffer_width = size.x;
    if (size.x > renderer_width - position.x) {
        size.x = renderer_width - position.x;
    }

    if (size.y > renderer_height - position.y) {
        size.y = renderer_height - position.y;
    }

    for (u32 py = 0; py < size.y; py++) {
        for (u32 px = 0; px < size.x; px++) {
            u64 src_index = (u64)(position.y + py) * renderer_width + (position.x + px);
            u64 dest_index = (u64)py * buffer_width + px;
            pixels[dest_index] = backbuffer[src_index];
        }
    }
}

void renderer_write_rect(Vector2u position, Size2u size, const u32* pixels) {
    if (pixels == NULL) {
        return;
    }

    if (position.x >= renderer_width || position.y >= renderer_height) {
        return;
    }

    u32 buffer_width = size.x;
    if (size.x > renderer_width - position.x) {
        size.x = renderer_width - position.x;
    }

    if (size.y > renderer_height - position.y) {
        size.y = renderer_height - position.y;
    }

    for (u32 py = 0; py < size.y; py++) {
        for (u32 px = 0; px < size.x; px++) {
            u64 src_index = (u64)py * buffer_width + px;
            u64 dest_index = (u64)(position.y + py) * renderer_width + (position.x + px);
            backbuffer[dest_index] = pixels[src_index];
        }
    }
}

void renderer_present(void) {
    for (u32 y = 0; y < renderer_height; y++) {
        u64 row_index = (u64)y * renderer_width;
        const u32* row = &backbuffer[row_index];

        framebuffer_write_row(y, row, renderer_width);
    }
}

void renderer_present_rect(Vector2u position, Size2u size) {
    if (position.x >= renderer_width || position.y >= renderer_height) {
        return;
    }

    if (size.x > renderer_width - position.x) {
        size.x = renderer_width - position.x;
    }

    if (size.y > renderer_height - position.y) {
        size.y = renderer_height - position.y;
    }

    for (u32 py = 0; py < size.y; py++) {
        u32 y = position.y + py;
        u64 index = (u64)y * renderer_width + position.x;

        const u32* row = &backbuffer[index];
        framebuffer_write_row_part(position.x, y, row, size.x);
    }
}