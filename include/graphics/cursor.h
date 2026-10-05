#ifndef CURSOR_H
#define CURSOR_H

#include <mouse/mouse.h>

/**
 * Initializes the graphical cursor.
 *
 * Sets the cursor position to the center of the framebuffer.
 * The cursor is not drawn automatically; call cursor_draw() after
 * initialization if it should be immediately visible.
 *
 * Must be called after framebuffer_init().
 */
void cursor_init(void);

/**
 * Updates the cursor position from a mouse event.
 *
 * Applies the relative X and Y movement reported by the mouse to the
 * cursor's absolute screen position. The Y delta is inverted because
 * PS/2 mouse coordinates grow upwards while framebuffer coordinates
 * grow downwards.
 *
 * The resulting position is clamped to the framebuffer bounds so the
 * entire cursor remains visible.
 *
 * The previous cursor image is currently erased by filling its area
 * with black before the new position is drawn. This means any content
 * underneath the cursor is overwritten. A future rendering/backbuffer
 * system should remove this limitation.
 *
 * Must be called after cursor_init().
 *
 * @param event Mouse event containing the relative movement.
 */
void cursor_handle_mouse(MouseEvent event);

/**
 * Draws the cursor at its current position.
 *
 * Renders the cursor directly into the framebuffer using the current
 * cursor dimensions and color.
 *
 * This function does not preserve the framebuffer contents underneath
 * the cursor.
 *
 * Requires a successfully initialized and mapped framebuffer.
 */
void cursor_draw(void);

#endif