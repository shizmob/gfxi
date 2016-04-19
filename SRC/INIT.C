#include <gfxi.h>
#include <intern/common.h>
#include <intern/bios.h>
#include <intern/gdc.h>

static void gfx_init_state(gfx_state_t* state);

/**
 * Initializes graphics library, sets GPU into given mode (if possible).
 * Returns 1 on success, 0 on failure.
 */
int gfx_init(gfx_state_t* state, gfx_mode_t mode)
{
	gfx_init_state(state);

	/* Detect devices first. We require at least *something* to work with. */
	gfx_detect_devices(state);
	if (!gfx_has_devices(state)) {
		gfx_set_error(state, "No graphical devices available");
		return 0;
	}

	/* Enter given graphics mode. */
	if (!gfx_enter_mode(state, mode)) {
		return 0;
	}

	/* Clock both GPUs to 5MHz. */
	outp(GDC_CONTROL2, GDC_CTL2_LOCK | 1);
	outp(GDC_CONTROL2, GDC_CTL2_TEXT_CLOCK | 1);
	outp(GDC_CONTROL2, GDC_CTL2_GFX_CLOCK  | 1);
	outp(GDC_CONTROL2, GDC_CTL2_LOCK | 0);

	/* Set planes. */
	gfx_set_plane(state, GFX_PLANE_DISPLAY,   0);
	gfx_set_plane(state, GFX_PLANE_RENDERING, 1);
	
	/* Set shape. */
	gfx_cmd_gfx(state, GDC_CMD_CURSOR_SHAPE, 1, 0x0);
	
	/* Save old palette. */
	gfx_store_palette(state, state->old_palette, 16);

	return 1;
}

/**
 * Initialize state with default values.
 */
static void gfx_init_state(gfx_state_t* state)
{
	state->last_error = NULL;
	state->devices = 0;
	state->devices_enabled = 0;
	state->mode = GFX_MODE_NONE;
	state->draw_mode = GFX_DRAW_MODE_SET;
	state->width = 0;
	state->height = 0;
}

/**
 * Disables graphical co-GPU and returns to text mode.
 */
int gfx_fini(gfx_state_t* state)
{
	/* Re-enter text mode. */
	if (!gfx_enter_mode(state, GFX_MODE_TEXT)) {
		return 0;
	}
	
	/* Clock both GPUs back to 2.5MHz. */
	outp(GDC_CONTROL2, GDC_CTL2_LOCK | 1);
	outp(GDC_CONTROL2, GDC_CTL2_TEXT_CLOCK | 0);
	outp(GDC_CONTROL2, GDC_CTL2_TEXT_CLOCK | 0);
	outp(GDC_CONTROL2, GDC_CTL2_LOCK | 0);

	/* Restore old palette. */
	gfx_fill_palette(state, state->old_palette, 16);
	
	return 1;
}