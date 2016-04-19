#include <gfxi.h>
#include <intern/common.h>
#include <intern/gdc.h>
#include <intern/misc.h>

void gdc_wait_until_ready(gfx_state_t* state)
{
	unsigned target;

	/* Figure out which GPU to bother. */
	if (state->mode == GFX_MODE_TEXT) {
		target = GDC_TEXT_STATUS;
	} else {
		target = GDC_GFX_STATUS;
	}
	
	while (inp(target) & GDC_STATUS_FULL) {
		/* Wait 1.2ms to not bother the GPU too much. */
		outp(MISC_WAIT, 0);
		outp(MISC_WAIT, 0);
	}
}