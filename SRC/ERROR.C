#include <gfxi.h>

/**
 * Set last error that occurred.
 */
void gfx_set_error(gfx_state_t* state, const char* message)
{
	state->last_error = message;
}

/**
 * Retrieve last error.
 */
const char* gfx_last_error(gfx_state_t* state)
{
	return state->last_error;
}