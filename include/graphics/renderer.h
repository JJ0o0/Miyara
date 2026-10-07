#ifndef RENDERER_H
#define RENDERER_H

#include <graphics/color.h>
#include <math/vector.h>
#include <types/types.h>

/**
 * Initializes the software renderer.
 *
 * Allocates the renderer backbuffer using the current framebuffer dimensions.
 * The renderer stores pixels in the framebuffer's encoded 32-bit pixel format.
 *
 * @return true if the renderer was initialized successfully.
 * @return false if the backbuffer could not be allocated.
 *
 * @note The framebuffer and heap must already be initialized before calling
 *       this function.
 */
bool renderer_init(void);

/**
 * Returns the width of the renderer surface.
 *
 * @return Renderer width in pixels.
 */
u32 renderer_get_width(void);

/**
 * Returns the height of the renderer surface.
 *
 * @return Renderer height in pixels.
 */
u32 renderer_get_height(void);

/**
 * Writes a single pixel to the renderer backbuffer.
 *
 * The supplied color is encoded using the framebuffer pixel format before
 * being stored in the backbuffer.
 *
 * @param position Position of the pixel in renderer coordinates.
 * @param color    Color to write.
 *
 * @note If the position is outside the renderer surface, no pixel is written.
 */
void renderer_put_pixel(Vector2u position, Color color);

/**
 * Fills the entire renderer backbuffer with a single color.
 *
 * This operation only modifies the backbuffer. The framebuffer is not updated
 * until renderer_present() or another presentation function is called.
 *
 * @param color Color used to fill the backbuffer.
 */
void renderer_clear(Color color);

/**
 * Fills a rectangular region of the renderer backbuffer.
 *
 * Pixels outside the renderer surface are ignored through the renderer's
 * pixel bounds checks.
 *
 * @param position Top-left position of the rectangle.
 * @param size     Rectangle dimensions.
 * @param color    Color used to fill the rectangle.
 *
 * @note This operation modifies only the backbuffer.
 */
void renderer_fill_rect(Vector2u position, Size2u size, Color color);

/**
 * Copies a rectangular region from the renderer backbuffer into an external
 * pixel buffer.
 *
 * Pixels are copied in row-major order. The destination buffer uses the
 * requested rectangle width as its row stride, even if the visible region
 * must be clipped against the renderer boundaries.
 *
 * @param position Top-left position of the region to read.
 * @param size     Requested region dimensions.
 * @param pixels   Destination buffer for encoded pixel values.
 *
 * @note The destination buffer must be large enough to hold
 *       size.x * size.y pixels.
 * @note If pixels is NULL or the starting position is outside the renderer,
 *       no data is copied.
 */
void renderer_read_rect(Vector2u position, Size2u size, u32* pixels);

/**
 * Copies a rectangular region from an external pixel buffer into the renderer
 * backbuffer.
 *
 * Pixels are read in row-major order. The source buffer uses the requested
 * rectangle width as its row stride, even if the visible region must be
 * clipped against the renderer boundaries.
 *
 * @param position Top-left destination position in the renderer.
 * @param size     Requested region dimensions.
 * @param pixels   Source buffer containing encoded pixel values.
 *
 * @note pixels must contain values already encoded in the framebuffer's
 *       native pixel format.
 * @note The source buffer must contain at least size.x * size.y pixels.
 * @note If pixels is NULL or the starting position is outside the renderer,
 *       no data is copied.
 */
void renderer_write_rect(Vector2u position, Size2u size, const u32* pixels);

void renderer_copy_rect(Vector2u src_position, Vector2u dest_position, Size2u size);

/**
 * Presents the entire renderer backbuffer to the framebuffer.
 *
 * Each renderer row is copied to the corresponding framebuffer scanline.
 * This is intended for full-screen updates.
 *
 * @note This operation may be expensive because the entire renderer surface
 *       is transferred to the framebuffer.
 */
void renderer_present(void);

/**
 * Presents a rectangular region of the renderer backbuffer to the framebuffer.
 *
 * Only the specified region is transferred, making this suitable for small
 * updates such as cursor movement and other localized redraws.
 *
 * The requested region is clipped against the renderer boundaries.
 *
 * @param position Top-left position of the region to present.
 * @param size     Region dimensions.
 */
void renderer_present_rect(Vector2u position, Size2u size);

#endif