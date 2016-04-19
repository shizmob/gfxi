#include <stdlib.h>
#include <gfxi.h>
#include <intern/common.h>
#include <intern/gdc.h>

/**
 * Fill `len` * 3-sized buffer with current palette values up to `len` colors.
 */
void gfx_store_palette(gfx_state_t* state, unsigned* palette, size_t len)
{
	size_t i;
	
	for (i = 0; i < len; i++) {
		outp(GDC_PALETTE_INDEX, i);
		*palette++ = inp(GDC_PALETTE_R);
		*palette++ = inp(GDC_PALETTE_G);
		*palette++ = inp(GDC_PALETTE_B);
	}
}

/**
 * Fill `len`-color palette with color values.
 * `buf` should be a 3 * `len` size buffer containing R, G, and B values for every index.
 */
void gfx_fill_palette(gfx_state_t* state, unsigned* palette, size_t len)
{
	size_t i;
	
	for (i = 0; i < len; i++) {
		outp(GDC_PALETTE_INDEX, i);
		outp(GDC_PALETTE_R, *palette++);
		outp(GDC_PALETTE_G, *palette++);
		outp(GDC_PALETTE_B, *palette++);
	}
}

/**
 * Fill `len`-size palette with alpha color values.
 * `buf` should be a 4 * len size buffer containing A, R, G, and B values for every index.
 */
void gfx_fill_palette_alpha(gfx_state_t* state, unsigned* buf, size_t len)
{
	int alpha;
	size_t i;

	for (i = 0; i < len; i++) {
		/* Tell the GPU the palette index we're working with. */
		outp(GDC_PALETTE_INDEX, i);
		/* Write R, G, and B values to the palette values register. */
		alpha = *buf++;
		outp(GDC_PALETTE_R, *buf++ / alpha);
		outp(GDC_PALETTE_G, *buf++ / alpha);
		outp(GDC_PALETTE_B, *buf++ / alpha);
	}
}

/**
 * Reset `len`-size palette to black.
 */
void gfx_clear_palette(gfx_state_t* state, size_t len)
{
	unsigned* buf = calloc(sizeof(unsigned), len * 4);
	if (buf == NULL) {
		return;
	}

	/* calloc() already zero-fills, so every color will be #00000000. */
	gfx_fill_palette(state, buf, len);
	free(buf);
}
