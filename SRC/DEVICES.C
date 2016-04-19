#include <gfxi.h>
#include <intern/common.h>
#include <intern/gdc.h>

/**
 * Detect devices available on this system.
 */
void gfx_detect_devices(gfx_state_t* state)
{
	int devices = 0;
	
	/* XXX: Not sure of a way to detect the two GDCs, so presume they exist. */
	devices |= GFX_DEVICE_GDC_TEXT;
	devices |= GFX_DEVICE_GDC_GFX;
	
	
	state->devices = devices;
}

/**
 * Check if any devices were detected.
 */
int gfx_has_devices(gfx_state_t* state)
{
    return state->devices != 0;
}

/**
 * Check if given device was detected.
 */
int gfx_has_device(gfx_state_t* state, gfx_device_t device)
{
	return (state->devices & device) > 0;
}

/**
 * Check if given device is enabled.
 */
int gfx_device_enabled(gfx_state_t* state, gfx_device_t device)
{
	return (state->devices_enabled & device) > 0;
}

/**
 * Enable given device. Returns 0 on failure, 1 on success.
 */
int gfx_enable_device(gfx_state_t* state, gfx_device_t device)
{
	if (!gfx_has_device(state, device)) {
		gfx_set_error(state, "Device not present");
		return 0;
	}
	if (gfx_device_enabled(state, device)) {
		gfx_set_error(state, "Device already enabled");
		return 0;
	}
	
	switch (device) {
		case GFX_DEVICE_GDC_TEXT:
			outp(GDC_TEXT_COMMAND, GDC_CMD_START);
			break;
		case GFX_DEVICE_GDC_GFX:
			outp(GDC_GFX_COMMAND, GDC_CMD_START);
			break;
		case GFX_DEVICE_GRCG:
			break;
		case GFX_DEVICE_EGC:
			break;
	}
	
	state->devices_enabled |= device;
	return 1;
}

/**
 * Disable given device. Returns 0 on failure, 1 on succes.
 */
int gfx_disable_device(gfx_state_t* state, gfx_device_t device)
{
	if (!gfx_has_device(state, device)) {
		gfx_set_error(state, "Device not present");
		return 0;
	}
	if (!gfx_device_enabled(state, device)) {
		gfx_set_error(state, "Device not enabled");
		return 0;
	}
	
	switch (device) {
		case GFX_DEVICE_GDC_TEXT:
			outp(GDC_TEXT_COMMAND, GDC_CMD_STOP);
			break;
		case GFX_DEVICE_GDC_GFX:
			outp(GDC_GFX_COMMAND, GDC_CMD_STOP);
			break;
		case GFX_DEVICE_GRCG:
			break;
		case GFX_DEVICE_EGC:
			break;
	}
	
	state->devices_enabled &= ~device;
	return 1;
}