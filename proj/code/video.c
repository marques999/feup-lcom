#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "input.h"
#include "video.h"
#include "vbe.h"

#define VIDEO_INTERRUPT 0x10
#define VBE_CALL 0x4F
#define VBE_GET_MODE_INFO 0x01
#define VBE_SET_MODE 0x02
#define LINEAR_MODEL (1<<14)

#define make_rgb565(r, g, b) ((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11))
#define swap(x,y) { int z = x; x = y; y = z; }

#define BLUE(pixel) (((pixel & 0xf800) >> 11) << 3)
#define GREEN(pixel) (((pixel & 0x07e0) >> 5) << 2)
#define RED(pixel) (((pixel & 0x001f)) << 3)

#define MODE_1024X768_16BPP 0x117

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

static char *video_mem; /* Process address to which VRAM is mapped */
static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned vram_size;

void draw_pixel(int x, int y, short color, short* buffer) {
	if (x < h_res && y < v_res) {
		*(buffer + (y << 10) + x) = color;
	}
}

short pixel_blend(short src, short dst, unsigned char alpha) {
	int r = ((alpha * RED(dst) + (255 - alpha) * RED(src)) >> 8);
	int g = ((alpha * GREEN(dst) + (255 - alpha) * GREEN(src)) >> 8);
	int b = ((alpha * BLUE(dst) + (255 - alpha) * BLUE(src)) >> 8);
	return make_rgb565(r, g, b);
}

extern void memset16(short* buffer, uint16_t n, int32_t c);

__inline short* buffer_create() {
	return ((short*) malloc(vram_size));
}

__inline void buffer_copy(short* src, short* dst) {
	memcpy((char*) dst, (char*) src, vram_size);
}

__inline void buffer_render(short* src) {
	memcpy((char*) video_mem, (char*) src, vram_size);
}

__inline void buffer_fill(short* src) {
	memset16(src, 0, v_res*h_res);
}

void buffer_blend(short* src, short* dst, unsigned char alpha) {
	int x;
	int y;
	int i = 0;
	for (y = 0; y < v_res; y++) {
		for (x = 0; x < h_res; i++, x++) {
			dst[i] = pixel_blend(src[i], dst[i], alpha);
		}
	}
}

void buffer_destroy(short* src) {

	if (src == NULL) {
		return;
	}

	free(src);
	src = NULL;
}

font_t* font_unpack(const char* filename) {

	// allocating necessary size
	font_t* newFont = malloc(sizeof(font_t));

	// open filename in read binary mode
	FILE *fp = fopen(filename, "rb");

	if (fp == NULL) {
		video_exit();
		printf("arkanix::resource not found %s\n", filename);
		exit(1);
	}

	// read the bitmap file header
	fread(&newFont->size, sizeof(int), 1, fp);
	fread(&newFont->width, sizeof(int), 1, fp);

	int fontSizeBytes;
	fread(&fontSizeBytes, sizeof(int), 1, fp);

	// read glyph dimensions
	if (!newFont->size) {
		return NULL;
	}

	newFont->chars = (glyph_t*) calloc(1, sizeof(glyph_t) * newFont->size);

	int i;
	for (i = 0; i < newFont->size; i++) {
		fread(&newFont->chars[i].width, sizeof(short), 1, fp);
		fread(&newFont->chars[i].height, sizeof(short), 1, fp);
	}

	short* bitmapData = (short*) malloc(fontSizeBytes);
	fread(bitmapData, fontSizeBytes, 1, fp);

	// allocate enough memory for the bitmap image data

	for (i = 0; i < newFont->size; i++) {

		int glyphHeight = newFont->chars[i].height;
		int glyphWidth = newFont->chars[i].width;
		int glyphSize = glyphWidth * glyphHeight * sizeof(short);

		short* glyphData = (short*) malloc(glyphSize);
		int glyphLocation = glyphHeight - 1;
		short* bitmapPosition = bitmapData;
		short* glyphPosition = (short*) glyphData + glyphLocation * glyphWidth;

		int k;
		for (k = 0; k < glyphHeight; k++) {
			memcpy((char*) glyphPosition, (char*) bitmapPosition, glyphWidth << 1);
			bitmapPosition += newFont->width;
			glyphPosition -= glyphWidth;
		}

		bitmapData += glyphWidth;
		newFont->chars[i].data = glyphData;
	}

	// close file and return bitmap image data
	fclose(fp);
	return newFont;
}

void font_destroy(font_t* font) {

	if (font == NULL) {
		return;
	}

	free(font->chars);
	free(font);
	font = NULL;
}

void glyph_draw(font_t* font, int glyphIndex, int xi, int yi, short* buffer) {

	glyph_t current_glyph = font->chars[glyphIndex];

	int bitmapPos = 0;
	int x, y;

	for (y = yi; y < yi + current_glyph.height; y++) {
		for (x = xi; x < xi + current_glyph.width; x++) {
			if (current_glyph.data[bitmapPos] != (short) 0xF81F) {
				draw_pixel(x, y, current_glyph.data[bitmapPos], buffer);
			}
			bitmapPos++;
		}
	}
}

void write_string(font_t* font, int xi, int yi, char* string, short* buffer) {

	int stringPos = 0;

	while (string[stringPos] != '\0') {

		int glyphIndex = string[stringPos] - '0';

		if (string[stringPos] == 0x20) {
			xi += 8;
		} else if (glyphIndex >= font->size) {
		} else {
			glyph_draw(font, glyphIndex, xi, yi, buffer);
			xi += font->chars[glyphIndex].width;
		}

		stringPos++;
	}
}

void write_number(font_t* font, int xi, int yi, int value, int n, short* buffer) {

	char buf[8];
	sprintf(buf, "%0*d", n, value);
	write_string(font, xi, yi, buf, buffer);
}

__inline int video_check_bounds(unsigned short x, unsigned short y) {
	return (x < h_res && y < v_res) ? 1 : 0;
}

__inline unsigned video_width() {
	return h_res;
}

__inline unsigned video_height() {
	return v_res;
}

void* video_init() {

	struct mem_range mr;
	struct reg86u reg86;

	vbe_mode_info_t* vmi_p = malloc(sizeof(vbe_mode_info_t));

	reg86.u.b.intno = VIDEO_INTERRUPT;
	reg86.u.b.ah = VBE_CALL;
	reg86.u.b.al = VBE_SET_MODE;
	reg86.u.w.bx = LINEAR_MODEL | MODE_1024X768_16BPP;

	if (sys_int86(&reg86) != OK) {
		return NULL;
	}

	if (vbe_get_mode_info(MODE_1024X768_16BPP, vmi_p) != OK) {
		return NULL;
	}

	h_res = vmi_p->XResolution;
	v_res = vmi_p->YResolution;
	bits_per_pixel = vmi_p->BitsPerPixel;
	vram_size = h_res * v_res * (bits_per_pixel >> 3);

	mr.mr_base = vmi_p->PhysBasePtr;
	mr.mr_limit = mr.mr_base + vram_size;

	if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK) {
		return NULL;
	}

	video_mem = vm_map_phys(SELF, (void*) mr.mr_base, vram_size);

	if (video_mem == MAP_FAILED) {
		return NULL;
	}

	return video_mem;
}

void video_exit() {

	struct reg86u reg86;

	reg86.u.b.intno = 0x10;
	reg86.u.b.ah = 0x00;
	reg86.u.b.al = 0x03;

	sys_int86(&reg86);
}

rectangle_t* create_rectangle(int xi, int yi, int xf, int yf) {

	rectangle_t* new_rectangle = malloc(sizeof(rectangle_t));

	new_rectangle->xi = xi;
	new_rectangle->yi = yi;
	new_rectangle->xf = xf;
	new_rectangle->yf = yf;

	return new_rectangle;
}

__inline void draw_horizontal_line(unsigned xi, unsigned yi, unsigned size, short color, short* buffer) {
	memset16(buffer + (yi << 10) + xi, color, size);
}

void draw_rectangle_fill(rectangle_t* rect, short* buffer, short color) {

	int y;

	if (rect->xf < rect->xi) {
		swap(rect->xi, rect->xf);
	}

	if (rect->yf < rect->yi) {
		swap(rect->yi, rect->yf);
	}

	for (y = rect->yi; y < rect->yf; y++) {
		draw_horizontal_line(rect->xi, y, rect->xf - rect->xi, color, buffer);
	}
}

void draw_rectangle(rectangle_t* rect, short* buffer, short color) {

	int y;

	if (rect->xf < rect->xi) {
		swap(rect->xi, rect->xf);
	}

	if (rect->yf < rect->yi) {
		swap(rect->yi, rect->yf);
	}

	draw_horizontal_line(rect->xi, rect->yi, rect->xf - rect->xi, color, buffer);
	draw_horizontal_line(rect->xi, rect->yf, rect->xf - rect->xi, color, buffer);

	for (y = rect->yi; y < rect->yf; y++) {
		draw_pixel(rect->xi, y, color, buffer);
		draw_pixel(rect->xf, y, color, buffer);
	}
}
