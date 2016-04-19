#include <gfxi.h>
#include <intern/common.h>

/**
 * Clear screen.
 */
void gfx_clear(gfx_state_t* state, unsigned color)
{
	gfx_fill_range(state, 0, 0, state->width - 1, state->height - 1, color);
}

/**
 * Draw a rectangle.
 */
void gfx_draw_rectangle(gfx_state_t* state, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned color)
{
	while (y1 <= y2) {
		/* Fill line by line. */
		gfx_fill_range(state, x1, y1, x2, y1, color);
		y1++;
	}
}