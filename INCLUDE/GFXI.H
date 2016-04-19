#include <stddef.h>

typedef enum gfx_device
{
	GFX_DEVICE_GDC_TEXT = 0x1,
	GFX_DEVICE_GDC_GFX  = 0x2,
	GFX_DEVICE_GRCG     = 0x4,
	GFX_DEVICE_EGC      = 0x8
} gfx_device_t;

typedef enum gfx_mode
{
	GFX_MODE_NONE,
	GFX_MODE_TEXT,
	GFX_MODE_640x200x8,
	GFX_MODE_640x200x16,
	GFX_MODE_640x400x8,
	GFX_MODE_640x400x16,
	GFX_MODE_640x400x256
} gfx_mode_t;

typedef enum gfx_plane
{
	GFX_PLANE_DISPLAY   = 0,
	GFX_PLANE_RENDERING = 2
} gfx_plane_t;

typedef enum gfx_draw_mode
{
	GFX_DRAW_MODE_SET,
	GFX_DRAW_MODE_AND,
	GFX_DRAW_MODE_OR,
	GFX_DRAW_MODE_XOR
} gfx_draw_mode_t;

typedef struct gfx_state
{
	const char* last_error;

	int devices;
	int devices_enabled;

	gfx_mode_t mode;
	gfx_draw_mode_t draw_mode;
	unsigned width;
	unsigned height;

	unsigned old_palette[16 * 3];
} gfx_state_t;


/* Initialization/finalization; src/init.c */
int          gfx_init(gfx_state_t* state, gfx_mode_t mode);
int          gfx_fini(gfx_state_t* state);

/* Error retrieval/setting: src/error.c */
const char*  gfx_last_error(gfx_state_t* state);
void         gfx_set_error(gfx_state_t*, const char* message);

/* Device detection/manipulation: src/devices.c */
void         gfx_detect_devices(gfx_state_t* state);
int          gfx_has_devices(gfx_state_t* state);
int          gfx_has_device(gfx_state_t* state, gfx_device_t device);
int          gfx_device_enabled(gfx_state_t* state, gfx_device_t device);
int          gfx_enable_device(gfx_state_t* state, gfx_device_t device);
int          gfx_disable_device(gfx_state_t* state, gfx_device_t device);

/* Mode manipulation: src/mode.c */
int          gfx_enter_mode(gfx_state_t* state, gfx_mode_t mode);
gfx_mode_t   gfx_get_mode(gfx_state_t);

/* Palette manipulation: src/palette.c */
void         gfx_store_palette(gfx_state_t* state, unsigned* palette, size_t ncolors);
void         gfx_fill_palette(gfx_state_t* state, unsigned* palette, size_t ncolors);
void         gfx_fill_palette_alpha(gfx_state_t* state, unsigned* palette, size_t ncolors);
void         gfx_clear_palette(gfx_state_t* state, size_t ncolors);

/* Plane manipulation: src/planes.c */
void         gfx_set_plane(gfx_state_t* state, gfx_plane_t plane, int which);
int          gfx_get_plane(gfx_state_t* state, gfx_plane_t plane);
void         gfx_swap_planes(gfx_state_t* state);

/* GDC commands: src/cmd.c */
void         gfx_cmd_text(gfx_state_t* state, unsigned command, size_t nargs, ...);
void         gfx_cmd_gfx(gfx_state_t* state,  unsigned command, size_t nargs, ...);
void         gfx_wait_vsync(gfx_state_t* state);

/* Direct pixel and pixel range manipulation: src/pixel.c */
void         gfx_set_pixel(gfx_state_t* state, unsigned x, unsigned y, unsigned color);
unsigned     gfx_get_pixel(gfx_state_t* state, unsigned x, unsigned y);
void         gfx_set_range(gfx_state_t* state, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned* buf);
void         gfx_get_range(gfx_state_t* state, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned* buf);
void         gfx_fill_range(gfx_state_t* state, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned color);

/* Higher level drawing: src/shapes.c */
void         gfx_clear(gfx_state_t* state, unsigned color);
void         gfx_draw_rectangle(gfx_state_t* state, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned color);