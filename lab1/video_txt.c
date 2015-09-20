#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem; /* Address to which VRAM is mapped */

static unsigned scr_width; /* Width of screen in columns */
static unsigned scr_lines; /* Height of screen in lines */

void vt_fill(char ch, char attr) {
	char* vptr;
	vptr = video_mem;
	int i;
	for (i = 0; i < scr_lines * scr_width; i++) {
		*(vptr++) = ch;
		*(vptr++) = attr;
	}
}

void vt_blank() {
	vt_fill(0x20, 0x00);
}

int vt_print_char(char ch, char attr, int r, int c) {
	if (r >= scr_lines || c >= scr_width) {
		printf("ERROR: out of bounds (r = %d, c = %d)", r, c);
		return -1;
	}
	char* vptr;
	vptr = video_mem;
	int i;
	vptr += (r * scr_width + c) << 1;
	*(vptr++) = ch;
	*(vptr++) = attr;
	return 0;
}

int vt_print_string(char *str, char attr, int r, int c) {
	char* vptr;
	vptr = video_mem;
	int i = 0;
	int len = strlen(str);
	if (r >= scr_lines || c >= scr_width
			|| (scr_width * r + c + len) >= (scr_width * scr_lines)) {
		printf("ERROR: out of bounds (r = %d, c = %d)", r, c);
		return -1;
	}
	vptr += (r * scr_width + c) << 1;
	for (i = 0; i < len; i++) {
		*(vptr++) = str[i];
		*(vptr++) = attr;
	}
	return 0;
}

int vt_print_int(int num, char attr, int r, int c) {
	int i = 0;
	char temp[32];
	char buf[32];
	do {
		buf[i++] = '0' + num % 10;
		num /= 10;
	} while (num != 0);
	for (c = 0; i != 0; c++, --i) {
		temp[c] = buf[i-1];
	}
	vt_print_string(temp, attr, r, c);
}

int vt_draw_frame(int width, int height, char attr, int r, int c) {
	char* vptr;
	vptr = video_mem;
	int i, j;
	char ch;
	if (r >= scr_lines || c >= scr_width) {
		printf("ERROR: out of bounds (r = %d, c = %d)", r, c);
		return -1;
	}
	vptr += (r * scr_width + c) << 1;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (i == 0) {
				if (j == 0)
					ch = UL_CORNER;
				else if (j == width - 1)
					ch = UR_CORNER;
				else
					ch = HOR_BAR;
			} else if (i == height - 1) {
				if (j == 0)
					ch = LL_CORNER;
				else if (j == width - 1)
					ch = LR_CORNER;
				else
					ch = HOR_BAR;
			} else {
				if (j == 0 || j == width - 1)
					ch = VERT_BAR;
				else
					ch = 0x20;
			}
			*(vptr++) = ch;
			*(vptr++) = attr;
		}
		vptr += (scr_width - width) << 1;
	}
	return 0;
}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p) {

	int r;
	struct mem_range mr;

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes)(vi_p->vram_base);
	mr.mr_limit = mr.mr_base + vi_p->vram_size;

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vi_p->vram_size);

	if (video_mem == MAP_FAILED)
		panic("video_txt couldn't map video memory");

	/* Save text mode resolution */

	scr_lines = vi_p->scr_lines;
	scr_width = vi_p->scr_width;

	return video_mem;
}
