#include <gfxi.h>
#include <intern/common.h>
#include <intern/gdc.h>

/**
 * Wait for vsync.
 */
void gfx_wait_vsync(gfx_state_t* state)
{
	/* Wait until vsync occurs. */
	while (!(inp(GDC_GFX_STATUS) & GDC_STATUS_VSYNC)) {
	
	}
	/* Now wait until it's over. */
	while (inp(GDC_GFX_STATUS)   & GDC_STATUS_VSYNC) {
	
	}
}