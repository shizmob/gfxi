#include <gfxi.h>
#include <intern/common.h>
#include <intern/gdc.h>

/**
 * Set plane.
 */
void gfx_set_plane(gfx_state_t* state, gfx_plane_t plane, int which)
{
	outp(GDC_SET_DISP_PLANE + plane, which);
}

/**
 * Get plane.
 */
int gfx_get_plane(gfx_state_t* state, gfx_plane_t plane)
{
	return inp(GDC_GET_DISP_PLANE + plane);
}

/**
 * Swap display and rendering planes.
 */
void gfx_swap_planes(gfx_state_t* state)
{
	gfx_set_plane(state, GFX_PLANE_DISPLAY,   1 - gfx_get_plane(state, GFX_PLANE_DISPLAY));
	gfx_set_plane(state, GFX_PLANE_RENDERING, 1 - gfx_get_plane(state, GFX_PLANE_RENDERING));
}
