#include <stdarg.h>
#include <gfxi.h>
#include <intern/common.h>
#include <intern/gdc.h>

/**
 * Perform GDC command.
 */
void gfx_cmd(gfx_state_t* state, unsigned offset, unsigned command, size_t nargs, va_list ap)
{
	outp(offset + 2, command);
	while (nargs--) {
		outp(offset, va_arg(ap, unsigned));
	}
}

/**
 * Perform text GDC command.
 */
void gfx_cmd_text(gfx_state_t* state, unsigned command, size_t nargs, ...)
{
	va_list ap;
	va_start(ap, nargs);
	gfx_cmd(state, GDC_TEXT_PARAM, command, nargs, ap);
	va_end(ap);
}

/**
 * Perform graphical GDC command.
 */
void gfx_cmd_gfx(gfx_state_t* state, unsigned command, size_t nargs, ...)
{
	va_list ap;
	va_start(ap, nargs);
	gfx_cmd(state, GDC_GFX_PARAM, command, nargs, ap);
	va_end(ap);
}
