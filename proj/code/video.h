#ifndef __VIDEO_H
#define __VIDEO_H

#include "bitmap.h"

#define VIDEO_INTERRUPT 0x10
#define VBE_CALL 0x4F
#define VBE_GET_MODE_INFO 0x01
#define VBE_SET_MODE 0x02
#define LINEAR_MODEL (1<<14)

typedef struct {

	short* data;
	int width;
	int height;

} glyph_t;

typedef struct {

	glyph_t* chars;
	int size;
	int width;

} font_t;

typedef struct {

	int xi;
	int yi;
	int xf;
	int yf;

} rectangle_t;

/*
 * Pixel plot and blend functions
 */
void draw_pixel(int x, int y, short color, short* buffer);
short pixel_blend(short src, short dst, unsigned char alpha);

/*
 * Double Buffering
 */
short* buffer_create();
void buffer_copy(short* src, short* dst);
void buffer_render(short* src);
void buffer_fill(short* src);
void buffer_blend(short* src, short* dst, unsigned char alpha);
void buffer_destroy(short* src);

/*
 * Font loading/drawing/writing functions
 */
font_t* font_unpack(const char* filename);
void font_destroy(font_t* font);
void glyph_draw(font_t* font, int glyphIndex, int xi, int yi, short* buffer);
void write_string(font_t* font, int xi, int yi, char* string, short* buffer);
void write_number(font_t* font, int xi, int yi, int value, int n, short* buffer);

/*
 * Auxilliary graphics functions
 */
int video_check_bounds(unsigned short x, unsigned short y);
unsigned video_width();
unsigned video_height();

/*
 * VESA mode switch functions
 */
void* video_init(void);
void video_exit(void);

/*
 * Rectangle class
 */
rectangle_t* create_rectangle(int xi, int yi, int xf, int yf);
void draw_rectangle(rectangle_t* rect, short* buffer, short color);
void draw_rectangle_fill(rectangle_t* rect, short* buffer, short color);

#endif /* __VIDEO_H */
