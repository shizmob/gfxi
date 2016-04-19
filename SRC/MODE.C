#include <gfxi.h>
#include <intern/common.h>
#include <intern/bios.h>
#include <intern/gdc.h>

/**
 * Attempt to enter specified graphics mode. Return 1 on success, 0 on failure.
 */
int gfx_enter_mode(gfx_state_t* state, gfx_mode_t mode)
{
	union REGS in, out;

	/* Sanity checks. */
	if (mode == GFX_MODE_NONE) {
		gfx_set_error(state, "Invalid mode: GFX_MODE_NONE");
		return 0;
	}
	if (mode > GFX_MODE_TEXT && !gfx_has_device(state, GFX_DEVICE_GDC_GFX)) {
		gfx_set_error(state, "Non-text mode requested but graphical GDC not available");
		return 0;
	}
	if (mode == GFX_MODE_640x400x256 && !gfx_has_device(state, GFX_DEVICE_EGC)) {
		gfx_set_error(state, "256-color mode requested but EGC not available");
		return 0;
	}

	/* Set graphics mode.
	 * Bit 0-3 are unused.
	 * Bit 4 is the bank to use. We always use the front bank (0).
     * Bit 5 is whether to use colors (0) or not (1). We always use colors.
     * Bit 6 and 7 are the horizontal resolution: 1 is 200 lines, 3 is 400 lines.
	 */
	in.h.ah = VGA_MODE;
	in.h.ch = (mode >= GFX_MODE_640x400x8 ? 3 : 1) << 6;
	int86(INTERRUPT_VGA, &in, &out);
	in.h.ch = 0;
	
	/* Configure GDC. */
	if (gfx_has_device(state, GFX_DEVICE_GDC_TEXT) || gfx_has_device(state, GFX_DEVICE_GDC_GFX)) {
		/* Unlock settings. */
		outp(GDC_CONTROL2, GDC_CTL2_LOCK   | 1);
		/* Colors. 0: colors, 1: no colors */
		outp(GDC_CONTROL,  GDC_CTL_COLORS  | 0);
		/* Plane size. 0: 400 line, 1: 200 line */
		outp(GDC_CONTROL,  GDC_CTL_LINES   | (mode >= GFX_MODE_640x400x8 ? 0 : 1));
		/* 16 color mode. */
		outp(GDC_CONTROL2, GDC_CTL2_COLORS | ((mode == GFX_MODE_640x200x16 || mode >= GFX_MODE_640x400x16) ? 1 : 0));
		/* Lock settings. */
		outp(GDC_CONTROL2, GDC_CTL2_LOCK   | 0);

		/* Enable relevant co-GPU, disable the other. */
		if (mode != GFX_MODE_TEXT) {
			/* Enable graphics. */
			in.h.ah = VGA_START;
			int86(INTERRUPT_VGA, &in, &out);
			/* Start graphical co-GPU. */
			gfx_enable_device(state, GFX_DEVICE_GDC_GFX);
			
			/* Disable text. */
			in.h.ah = VGA_HIDE_TEXT;
			int86(INTERRUPT_VGA, &in, &out);
			/* Stop text co-GPU. */
			gfx_disable_device(state, GFX_DEVICE_GDC_TEXT);

			/* Set graphical co-GPU as vsync master and text co-GPU as vsync slave. */
			outp(GDC_GFX_COMMAND,  GDC_CMD_VSYNC_MASTER);
			outp(GDC_TEXT_COMMAND, GDC_CMD_VSYNC_SLAVE);
		} else {
			/* Enable text. */
			in.h.ah = VGA_SHOW_TEXT;
			int86(INTERRUPT_VGA, &in, &out);
			/* Start text co-GPU. */
			gfx_enable_device(state, GFX_DEVICE_GDC_TEXT);

			/* Disable graphics. */
			in.h.ah = VGA_STOP;
			int86(INTERRUPT_VGA, &in, &out);
			/* Stop graphical co-GPU. */
			gfx_disable_device(state, GFX_DEVICE_GDC_GFX);

			/* Set text co-GPU as vsync master and graphical co-GPU as vsync slave. */
			outp(GDC_TEXT_COMMAND, GDC_CMD_VSYNC_MASTER);
			outp(GDC_GFX_COMMAND,  GDC_CMD_VSYNC_SLAVE);
		}
	}
	
	/* Configure EGC. */
	if (gfx_has_device(state, GFX_DEVICE_EGC)) {
		/* Unlock settings. */
		outp(GDC_CONTROL2, GDC_CTL2_LOCK     | 1);
		/* 256-color mode.*/
		outp(GDC_CONTROL2, GDC_CTL2_ENHANCED | (mode == GFX_MODE_640x400x256 ? 1 : 0));
		/* Extended VRAM lines for 256-color mode. */
		outp(GDC_CONTROL2, GDC_CTL2_LINES    | (mode == GFX_MODE_640x400x256 ? 1 : 0));
		/* Lock settings. */
		outp(GDC_CONTROL2, GDC_CTL2_LOCK     | 0);
	}
	
	state->mode = mode;
	if (mode > GFX_MODE_TEXT) {
		state->width = 640;
		if (mode >= GFX_MODE_640x400x8) {
			state->height = 400;
		} else if(mode >= GFX_MODE_640x200x8) {
			state->height = 200;
		}
	} else {
		state->width = 80;
		state->height = 25;
	}
	return 1;
}

/**
 * Return current mode.
 */
gfx_mode_t gfx_current_mode(gfx_state_t* state)
{
	return state->mode;
}