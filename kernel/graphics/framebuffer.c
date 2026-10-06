#include <graphics/framebuffer.h>

#include <memory/memory.h>
#include <memory/paging.h>
#include <memory/pmm.h>

#include <math/math.h>

static Framebuffer framebuffer;

bool framebuffer_init(MBIHeader *mbi) {
    TagHeader* current_tag = (TagHeader*)((u8*)mbi + 0x8);
    FramebufferTag* fb_tag = NULL;
    while (current_tag->type != 0) {
        if (current_tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            fb_tag = (FramebufferTag*)current_tag;
            if (fb_tag->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB || fb_tag->framebuffer_bpp != 32) {
                return false;
            }

            FramebufferRGBInfo* rgb_info = (FramebufferRGBInfo*)((u8*)fb_tag + sizeof(FramebufferTag));
            if (
                rgb_info->red_mask_size != 8 ||
                rgb_info->green_mask_size != 8 ||
                rgb_info->blue_mask_size != 8
            ) {
                return false;
            }

            framebuffer.physical_address = fb_tag->framebuffer_addr;
            framebuffer.width = fb_tag->framebuffer_width;
            framebuffer.height = fb_tag->framebuffer_height;
            framebuffer.pitch = fb_tag->framebuffer_pitch;
            framebuffer.bpp = fb_tag->framebuffer_bpp;
            framebuffer.rgb = *rgb_info;
            break;
        }

        u8* next = (u8*)current_tag + current_tag->size;
        u8 left = (u64)next % 8;

        u8 offset = 0;
        if (left != 0) {
            offset = 8 - left;
        }

        current_tag = (TagHeader*)(next + offset);
    }

    return fb_tag != NULL;
}

bool framebuffer_map(void) {
    u64 physical_start = align_down(framebuffer.physical_address, PAGE_SIZE);
    u64 offset = framebuffer.physical_address - physical_start;
    u64 size = (u64)framebuffer.pitch * framebuffer.height;
    u64 total_size = offset + size;
    u64 page_count = ceil_div(total_size, PAGE_SIZE);

    for (u64 i = 0; i < page_count; i++) {
        u64 physical = physical_start + (i * PAGE_SIZE);
        u64 virtual = FRAMEBUFFER_VIRTUAL_BASE + (i * PAGE_SIZE);

        if (!paging_map_page(virtual, physical, PAGE_PRESENT | PAGE_WRITABLE)) {
            return false;
        }
    }

    framebuffer.virtual_address = FRAMEBUFFER_VIRTUAL_BASE + offset;
    return true;
}

u32 framebuffer_get_width(void) {
    return framebuffer.width;
}

u32 framebuffer_get_height(void) {
    return framebuffer.height;
}

u32 framebuffer_encode_color(Color color) {
    u32 red = (u32)color.r << framebuffer.rgb.red_field_position;
    u32 green = (u32)color.g << framebuffer.rgb.green_field_position;
    u32 blue = (u32)color.b << framebuffer.rgb.blue_field_position;

    return red | green | blue;
}

void framebuffer_write_row(u32 y, const u32* pixels, u32 pixel_count) {
    if (y >= framebuffer.height || pixels == NULL) {
        return;
    }

    if (pixel_count > framebuffer.width) {
        pixel_count = framebuffer.width;
    }

    u64 line_address = framebuffer.virtual_address + ((u64)y * framebuffer.pitch);
    
    u32* dest = (u32*)line_address;
    for (u32 x = 0; x < pixel_count; x++) {
        dest[x] = pixels[x];
    }
}

void framebuffer_write_row_part(u32 x, u32 y, const u32* pixels, u32 pixel_count) {
    if (x >= framebuffer.width || y >= framebuffer.height || pixels == NULL) {
        return;
    }

    if (pixel_count > framebuffer.width - x) {
        pixel_count = framebuffer.width - x;
    }

    u64 line_address = framebuffer.virtual_address + ((u64)y * framebuffer.pitch) + ((u64)x * sizeof(u32));

    u32* dest = (u32*)line_address;
    for (u32 i = 0; i < pixel_count; i++) {
        dest[i] = pixels[i];
    }
}

void framebuffer_put_pixel(u32 x, u32 y, Color color) {
    if (x >= framebuffer.width || y >= framebuffer.height) {
        return;
    }

    u8 bytes_per_pixel = framebuffer.bpp / 8;
    u64 pixel_address = framebuffer.virtual_address + ((u64)y * framebuffer.pitch) + ((u64)x * bytes_per_pixel);
    u32 encoded_color = framebuffer_encode_color(color);

    u32* pixel = (u32*)pixel_address;
    *pixel = encoded_color;
}

void framebuffer_clear(Color color) {
    for (u32 y = 0; y < framebuffer.height; y++) {
        for (u32 x = 0; x < framebuffer.width; x++) {
            framebuffer_put_pixel(x, y, color);
        }
    }
}

void framebuffer_fill_rect(u32 x, u32 y, u32 width, u32 height, Color color) {
    for (u32 py = y; py < y + height; py++) {
        for (u32 px = x; px < x + width; px++) {
            framebuffer_put_pixel(px, py, color);
        }
    }
}