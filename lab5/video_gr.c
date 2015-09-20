#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "timer.h"
#include "vbe.h"

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned vram_size; /* Number of VRAM bytes */

/* Double buffering-related functions */

__inline void vg_create_buffer(char* buffer) {
	buffer = calloc(vram_size, 1);
}

__inline void vg_empty_buffer(char* buffer) {
	memset(buffer, 0, vram_size);
}

__inline void vg_write_vram(char* buffer) {
	memcpy(video_mem, buffer, vram_size);
}

/* Inline functions for video mode */

__inline void vg_draw_pixel(int x, int y, char color, char* buffer) {
	if (x < h_res && y < v_res) {
		*(buffer + (y << 10) + x) = color;
	}
}

__inline void vg_print_mode() {
	printf("video_mem = 0x%X, h_res = %d, v_res = %d", video_mem, h_res, v_res);
}

__inline int vg_check_bounds(unsigned short x, unsigned short y) {
	return (x > h_res || y > v_res) ? 0 : 1;
}

//

void* vg_init(unsigned short mode) {
	struct mem_range mr;
	struct reg86u reg86;

	vbe_mode_info_t* vmi_p = malloc(sizeof(vbe_mode_info_t));

	reg86.u.b.intno = VIDEO_INTERRUPT;
	reg86.u.b.ah = VBE_CALL;
	reg86.u.b.al = VBE_SET_MODE;
	reg86.u.w.bx = LINEAR_MODEL | mode;

	if (sys_int86(&reg86) != OK) {
		printf("vg_init()::bios call didn't return 0\n");
		return NULL;
	}

	if (vbe_get_mode_info(mode, vmi_p) != OK) {
		printf("vg_init():get_mode_info failed, couldn't get video mode information.\n");
		return NULL;
	}

	h_res = vmi_p->XResolution;
	v_res = vmi_p->YResolution;
	bits_per_pixel = vmi_p->BitsPerPixel;
	vram_size = h_res * v_res * (bits_per_pixel >> 3);

	/* Allow memory mapping */

	mr.mr_base = vmi_p->PhysBasePtr;
	mr.mr_limit = mr.mr_base + vram_size;

	if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK) {
		printf("vg_init()::sys_privctl didn't return 0.\n");
		return NULL;
	}

	/* Map video memory */

	video_mem = vm_map_phys(SELF, (void*) mr.mr_base, vram_size);

	if (video_mem == MAP_FAILED) {
		printf("vg_init()::vm_map_phys failed, coudln't allocate virtual memory.\n");
		return NULL;
	}

	return video_mem;
}

char *vg_read_xpm(char *map[], int *wd, int *ht) {
	int width, height, colors;
	char sym[256];
	int col;
	int i, j;
	char *pix, *pixtmp, *tmp, *line;
	char symbol;

	if (sscanf(map[0], "%d %d %d", &width, &height, &colors) != 3) {
		printf("read_xpm()::incorrect width, height, colors\n");
		return NULL;
	}
	if (width > h_res || height > v_res || colors > (1 << bits_per_pixel)) {
		printf("read_xpm()::incorrect width, height, colors\n");
		return NULL;
	}
	*wd = width;
	*ht = height;
	for (i = 0; i < 256; i++) {
		sym[i] = 0;
	}
	for (i = 0; i < colors; i++) {
		if (sscanf(map[i + 1], "%c %d", &symbol, &col) != 2) {
			printf("read_xpm()::incorrect symbol, color at line %d\n", i + 1);
			return NULL;
		}
		if (col > 256) {
			printf("read_xpm()::incorrect color at line %d\n", i + 1);
			return NULL;
		}
		sym[col] = symbol;
	}
	pix = pixtmp = malloc(width * height);
	for (i = 0; i < height; i++) {
		line = map[colors + 1 + i];
		for (j = 0; j < width; j++) {
			tmp = memchr(sym, line[j], 256);
			if (tmp == NULL) {
				printf("read_xpm():incorrect symbol at line %d, col %d\n", colors + i + 1, j);
				return NULL;
			}
			*pixtmp++ = tmp - sym;
		}
	}
	return pix;
}

void vg_draw_xpm(unsigned xi, unsigned yi, int width, int height, char* bytes) {
	int x, y;
	int i = 0;
	char* buffer = calloc(vram_size, 1);
	for (y = yi; y < yi + height; y++) {
		for (x = xi; x < xi + width; x++) {
			vg_draw_pixel(x, y, bytes[i++], buffer);
		}
	}
	memcpy(video_mem, buffer, vram_size);
	free(buffer);
}

void vg_draw_rectangle(unsigned xi, unsigned yi, unsigned xf, unsigned yf, char color) {
	int x, y;
	if (xf < xi) {
		int swap = xi;
		xi = xf;
		xf = swap;
	}
	if (yf < yi) {
		int swap = yi;
		yi = yf;
		yf = swap;
	}
	for (y = yi; y < yf; y++) {
		for (x = xi; x < xf; x++) {
			vg_draw_pixel(x, y, color, video_mem);
		}
	}
}

void vg_draw_line(unsigned xi, unsigned yi, unsigned xf, unsigned yf, char color) {
	float m;
	int i;

	int dx = xf - xi;
	int dy = yf - yi;

	int sign_x = dx > 0 ? 1 : -1;
	int sign_y = dy > 0 ? 1 : -1;

	if (abs(dx) >= abs(dy)) {
		m = (float) dy / dx;
		for (i = 0; i != dx; i += sign_x) {
			vg_draw_pixel(xi + i, yi + i * m, color, video_mem);
		}
	} else {
		m = dx / (float) dy;
		for (i = 0; i != dy; i += sign_y) {
			vg_draw_pixel(xi + i * m, yi + i, color, video_mem);
		}
	}
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */
	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("vg_exit()::bios call didn't return 0\n");
		return ERROR;
	}

	return OK;
}
