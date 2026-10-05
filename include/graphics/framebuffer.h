#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <multiboot/multiboot.h>
#include <graphics/color.h>
#include <types/types.h>

/**
 * Linear framebuffer description, filled in from the Multiboot2
 * framebuffer tag.
 *
 * Only 32-bit direct RGB framebuffers with 8 bits per channel are
 * accepted by framebuffer_init(), so after a successful init bpp is
 * always 32.
 */
typedef struct {
    /**
     * Physical address of the first pixel, as reported by the
     * bootloader.
     */
    u64 physical_address;

    /**
     * Virtual address of the first pixel. Set by framebuffer_map();
     * 0 until then.
     */
    u64 virtual_address;

    /**
     * Width of the visible area, in pixels.
     */
    u32 width;

    /**
     * Height of the visible area, in pixels.
     */
    u32 height;

    /**
     * Size of one scanline in bytes. May be larger than width times
     * the bytes per pixel, because of padding at the end of each line.
     */
    u32 pitch;

    /**
     * Bits per pixel.
     */
    u8  bpp;

    /**
     * Bit position of each color channel inside a pixel, as reported
     * by the bootloader. Every channel is 8 bits wide.
     */
    FramebufferRGBInfo rgb;
} Framebuffer;

/**
 * Reads the framebuffer description from the Multiboot2 info.
 *
 * Walks the tag list looking for the framebuffer tag and stores its
 * address and geometry. It does not map any memory: call
 * framebuffer_map() afterwards.
 *
 * Only direct RGB framebuffers with 32 bits per pixel and 8 bits per
 * color channel are supported. For anything else (indexed color, EGA
 * text, another bpp or channel size) it returns false without storing
 * anything.
 *
 * @param mbi Multiboot2 info structure passed by the bootloader.
 * @return true if a supported framebuffer tag was found, false if
 *         there is no framebuffer tag or its format is not supported.
 */
bool framebuffer_init(MBIHeader* mbi);

/**
 * Maps the framebuffer into the kernel's virtual address space.
 *
 * Maps every page covering the framebuffer (pitch * height bytes,
 * plus the offset of its first pixel within its page) as present and
 * writable starting at FRAMEBUFFER_VIRTUAL_BASE, and then sets
 * virtual_address. The pixel functions below only work after this
 * succeeds.
 *
 * Must be called once, after framebuffer_init() and paging_init().
 * Calling it again fails, because the pages are already mapped.
 *
 * On failure, the pages mapped before the error are kept (there is no
 * rollback) and virtual_address is left unset.
 *
 * @return true if all pages were mapped, false if a page could not be
 *         mapped (virtual address already in use, or no physical page
 *         for an intermediate table).
 */
bool framebuffer_map(void);

/**
 * Returns the width of the framebuffer.
 *
 * @return Width in pixels, or 0 if framebuffer_init() has not
 *         succeeded yet.
 */
u32 framebuffer_get_width(void);

/**
 * Returns the height of the framebuffer.
 *
 * @return Height in pixels, or 0 if framebuffer_init() has not
 *         succeeded yet.
 */
u32 framebuffer_get_height(void);

/**
 * Draws a single pixel.
 *
 * The origin (0, 0) is the top-left corner: x grows to the right and
 * y grows downwards. The color is converted to the framebuffer's
 * pixel format using the channel positions in Framebuffer.rgb, and
 * written as a single 32-bit value. Coordinates outside the screen
 * are ignored.
 *
 * Requires a successful framebuffer_map().
 *
 * @param x Column of the pixel.
 * @param y Row of the pixel.
 * @param color Color to draw.
 */
void framebuffer_put_pixel(u32 x, u32 y, Color color);

/**
 * Fills the whole visible area with a single color.
 *
 * The padding at the end of each scanline (see Framebuffer.pitch) is
 * not touched. Requires a successful framebuffer_map().
 *
 * @param color Color to fill the screen with.
 */
void framebuffer_clear(Color color);

/**
 * Fills a rectangle with a single color.
 *
 * Parts of the rectangle that fall outside the screen are clipped.
 * Does nothing if width or height is 0. Requires a successful
 * framebuffer_map().
 *
 * @param x Column of the top-left corner.
 * @param y Row of the top-left corner.
 * @param width Width of the rectangle, in pixels.
 * @param height Height of the rectangle, in pixels.
 * @param color Color to fill the rectangle with.
 */
void framebuffer_fill_rect(u32 x, u32 y, u32 width, u32 height, Color color);

#endif