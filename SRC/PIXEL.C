#include <gfxi.h>
#include <intern/gdc.h>

/**
 * Write a single pixel. Pretty inefficient.
 */
void gfx_set_pixel(gfx_state_t* state, unsigned x, unsigned y, unsigned color)
{
	/* Original:
	 *    (y * state->width + x) /  8
	 * -> (y * state->width + x) >> 3
	 */
	unsigned pos = y * (state->width >> 3) + (x >> 3);
	/* Original:
	 *    0b10000000 >> (x % 8)
	 * -> 127        >> (x % 8)
	 * -> 127        >> (x & 7)
	 */
	unsigned char bpos = 127 >> (x & 7);
	unsigned char npos = ~bpos;
	
	/* Write a bit of each color to different planes, depending on the draw mode.
	 * This involves some bit manipulation since we don't want to disturb other pixels in the same byte.
	 */
	switch (state->draw_mode) {
		case GFX_DRAW_MODE_SET:
			if (color & 1) {
				*(GDC_PLANE_1 + pos) |= bpos;
			} else {
				*(GDC_PLANE_1 + pos) &= npos;
			}
			if (color & 2) {
				*(GDC_PLANE_2 + pos) |= bpos;
			} else {
				*(GDC_PLANE_2 + pos) &= npos;
			}
			if (color & 4) {
				*(GDC_PLANE_3 + pos) |= bpos;
			} else {
				*(GDC_PLANE_3 + pos) &= npos;
			}
			if (color & 8) {
				*(GDC_PLANE_4 + pos) |= bpos;
			} else {
				*(GDC_PLANE_4 + pos) &= npos;
			}
			break;
		case GFX_DRAW_MODE_AND:
			if (!(color & 1)) {
				*(GDC_PLANE_1 + pos) &= npos;
			}
			if (!(color & 2)) {
				*(GDC_PLANE_2 + pos) &= npos;
			}
			if (!(color & 4)) {
				*(GDC_PLANE_3 + pos) &= npos;
			}
			if (!(color & 8)) {
				*(GDC_PLANE_4 + pos) &= npos;
			}
			break;
		case GFX_DRAW_MODE_OR:
			if (color & 1) {
				*(GDC_PLANE_1 + pos) |= bpos;
			}
			if (color & 2) {
				*(GDC_PLANE_2 + pos) |= bpos;
			}
			if (color & 4) {
				*(GDC_PLANE_3 + pos) |= bpos;
			}
			if (color & 8) {
				*(GDC_PLANE_4 + pos) |= bpos;
			}
			break;
		case GFX_DRAW_MODE_XOR:
			if (color & 1) {
				*(GDC_PLANE_1 + pos) ^= bpos;
			}
			if (color & 2) {
				*(GDC_PLANE_2 + pos) ^= bpos;
			}
			if (color & 4) {
				*(GDC_PLANE_3 + pos) ^= bpos;
			}
			if (color & 8) {
				*(GDC_PLANE_4 + pos) ^= bpos;
			}
			break;
	}
}

/**
 * Read a single pixel. Pretty inefficient.
 */
unsigned gfx_get_pixel(gfx_state_t* state, unsigned x, unsigned y)
{
	/* 8 pixels per byte are stored, so retrieve the relevant byte from all four planes
	 * and do a simple AND to check for our pixel.
	 */
	unsigned char res = 0;
	/* Original: see gfx_set_pixel(). */
	unsigned pos = y * (state->width >> 3) + (x >> 3);
	/* Original: see gfx_set_pixel(). */
	unsigned char bpos = 127 >> (x & 7);
	
	if (*(GDC_PLANE_1 + pos) & bpos) res |= 1;
	if (*(GDC_PLANE_2 + pos) & bpos) res |= 2;
	if (*(GDC_PLANE_3 + pos) & bpos) res |= 4;
	if (*(GDC_PLANE_4 + pos) & bpos) res |= 8;
	
	return res;
}

/**
 * Write a range of pixels.
 */
void gfx_set_range(gfx_state_t* state, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned* buf)
{
	unsigned n;
	unsigned r, g, b, e, c;
	unsigned long pos = y1 * (state->width >> 3) + (x1 >> 3);
	unsigned long end = y2 * (state->width >> 3) + (x2 >> 3);
	unsigned char far* r_plane = GDC_PLANE_1;
	unsigned char far* g_plane = GDC_PLANE_2;
	unsigned char far* b_plane = GDC_PLANE_3;
	unsigned char far* e_plane = GDC_PLANE_4;

	/* Fill pixels up until we can do whole-byte addressing. */
	if (x1 % 8) {
		while (x1 % 8 && (y1 < y2 || x1 <= x2)) {
			gfx_set_pixel(state, x1++, y1, *buf++);
		}
		pos++;
	}
	/* We only use the end position in whole-byte addressing; trim off the last part if it's not whole-byte. */
	if (x2 % 8) {
		end--;
	}

	/* Address bytes. */
	r_plane += pos;
	g_plane += pos;
	b_plane += pos;
	e_plane += pos;
	while (pos <= end) {
		/* Fetch colors from memory and store them in the appropriate format. */
		r = g = b = e = 0;
		for (n = 0; n < 8; n++) {
			c = *buf++;
			r |= (c & 1) << n;
			g |= (c & 2) << n;
			b |= (c & 4) << n;
			e |= (c & 8) << n;
		}

		switch (state->draw_mode) {
			case GFX_DRAW_MODE_SET:
				*r_plane++ = r;
				*g_plane++ = g;
				*b_plane++ = b;
				*e_plane++ = e;
				break;
			case GFX_DRAW_MODE_AND:
				*r_plane++ &= r;
				*g_plane++ &= g;
				*b_plane++ &= b;
				*e_plane++ &= e;
				break;
			case GFX_DRAW_MODE_OR:
				*r_plane++ |= r;
				*g_plane++ |= g;
				*b_plane++ |= b;
				*e_plane++ |= e;
				break;
			case GFX_DRAW_MODE_XOR:
				*r_plane++ ^= r;
				*g_plane++ ^= g;
				*b_plane++ ^= b;
				*e_plane++ ^= e;
				break;
		}
		pos++;
	}
	
	/* Address leftover pixels. */
	while (x2 % 8) {
		gfx_set_pixel(state, x2--, y2, *buf++);
	}
}

/**
 * Read a range of pixels.
 */
void gfx_get_range(gfx_state_t* state, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned* buf)
{

}

/**
 * Fill a range of pixels with a color.
 */
void gfx_fill_range(gfx_state_t* state, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned color)
{
	unsigned long pos = y1 * (state->width >> 3) + (x1 >> 3);
	unsigned long end = y2 * (state->width >> 3) + (x2 >> 3);
	unsigned char far* r_plane = GDC_PLANE_1;
	unsigned char far* g_plane = GDC_PLANE_2;
	unsigned char far* b_plane = GDC_PLANE_3;
	unsigned char far* e_plane = GDC_PLANE_4;
	unsigned r = color & 1 ? 255 : 0;
	unsigned g = color & 2 ? 255 : 0;
	unsigned b = color & 4 ? 255 : 0;
	unsigned e = color & 8 ? 255 : 0;

	/* Fill pixels up until we can do whole-byte addressing. */
	if (x1 % 8) {
		while (x1 % 8 && (y1 < y2 || x1 <= x2)) {
			gfx_set_pixel(state, x1++, y1, color);
		}
		pos++;
	}
	/* We only use the end position in whole-byte addressing; trim off the last part if it's not whole-byte. */
	if (x2 % 8) {
		end--;
	}

	/* Address bytes. */
	r_plane += pos;
	g_plane += pos;
	b_plane += pos;
	e_plane += pos;
	switch (state->draw_mode) {
		case GFX_DRAW_MODE_SET:
			while (pos <= end) {
				*r_plane++ = r;
				*g_plane++ = g;
				*b_plane++ = b;
				*e_plane++ = e;	
				pos++;
			}
			break;
		case GFX_DRAW_MODE_AND:
			while (pos <= end) {
				*r_plane++ &= r;
				*g_plane++ &= g;
				*b_plane++ &= b;
				*e_plane++ &= e;
				pos++;
			}
			break;
		case GFX_DRAW_MODE_OR:
			while (pos <= end) {
				*r_plane++ |= r;
				*g_plane++ |= g;
				*b_plane++ |= b;
				*e_plane++ |= e;
				pos++;
			}
			break;
		case GFX_DRAW_MODE_XOR:
			while (pos <= end) {
				*r_plane++ ^= r;
				*g_plane++ ^= g;
				*b_plane++ ^= b;
				*e_plane++ ^= e;
				pos++;
			}
			break;
	}
	
	/* Address leftover pixels. */
	while (x2 % 8) {
		gfx_set_pixel(state, x2--, y2, color);
	}
}