#include <graphics/framebuffer.h>
#include <log/log.h>

#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1

static Framebuffer framebuffer;

bool framebuffer_init(MBIHeader *mbi) {
    TagHeader* current_tag = (TagHeader*)((u8*)mbi + 0x8);
    FramebufferTag* fb_tag = NULL;
    while (current_tag->type != 0) {
        if (current_tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            fb_tag = (FramebufferTag*)current_tag;
            if (fb_tag->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
                return false;
            }

            framebuffer.address = fb_tag->framebuffer_addr;
            framebuffer.width = fb_tag->framebuffer_width;
            framebuffer.height = fb_tag->framebuffer_height;
            framebuffer.pitch = fb_tag->framebuffer_pitch;
            framebuffer.bpp = fb_tag->framebuffer_bpp;
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
