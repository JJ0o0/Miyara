#ifndef CURSOR_H
#define CURSOR_H

#include <mouse/mouse.h>

/**
 * Initializes the graphical mouse cursor.
 *
 * Places the cursor at the center of the current renderer area and
 * immediately presents it as an overlay.
 *
 * The cursor is rendered transiently: it is drawn into the renderer
 * backbuffer only long enough to present its region to the framebuffer,
 * then the original backbuffer pixels are restored. This keeps the
 * persistent scene free of cursor pixels.
 *
 * Must be called after the renderer and framebuffer are initialized.
 */
void cursor_init(void);

/**
 * Updates the cursor position from a mouse event.
 *
 * The previous cursor position is first restored on the physical
 * framebuffer from the clean renderer backbuffer. The relative mouse
 * movement is then applied to the cursor position.
 *
 * The Y delta is inverted because PS/2 mouse coordinates grow upwards,
 * while framebuffer coordinates grow downwards.
 *
 * The final position is clamped so the complete cursor remains inside
 * the renderer bounds.
 *
 * After moving, the cursor is presented again as an overlay at its new
 * position.
 *
 * Must be called after cursor_init().
 *
 * @param event Mouse event containing relative movement.
 */
void cursor_handle_mouse(MouseEvent event);

/**
 * Draws the cursor into the renderer backbuffer at its current position.
 *
 * This function only modifies the backbuffer. It does not present the
 * cursor to the framebuffer and does not restore the pixels underneath
 * it.
 *
 * It is mainly used internally by cursor_present_overlay().
 */
void cursor_draw(void);

/**
 * Presents the cursor as a transient overlay.
 *
 * The pixels underneath the cursor are first saved from the renderer
 * backbuffer. The cursor is then drawn and only its rectangle is
 * presented to the framebuffer.
 *
 * After presentation, the saved pixels are written back to the
 * backbuffer without presenting them, leaving the persistent scene
 * unchanged while the cursor remains visible on screen.
 *
 * Because later framebuffer presentations may overwrite the cursor,
 * this function should be called again after presenting scene content
 * that overlaps the cursor.
 */
void cursor_present_overlay(void);

#endif