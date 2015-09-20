#ifndef __BITMAP_H
#define __BITMAP_H

#include <stdint.h>
#include <stdio.h>

/**
 * @brief bitmap file header
 */
typedef struct
{
	uint16_t magic; ///> the magic number used to identify the bitmap file
	uint32_t filesz; ///> the size of the bitmap file in bytes
	uint32_t reserved; ///> reserved bytes
	uint32_t offset; ///> the starting address of the byte where the bitmap data can be found
} __attribute__((packed)) BitmapFileHeader;

/**
 * @brief bitmap info header
 */
typedef struct
{
	uint32_t header_sz; ///> size of this header (40 bytes)
	uint32_t width; ///> bitmap width in pixels
	uint32_t height; ///> bitmap height in pixels
	uint16_t nplanes; ///> number of color planes being used
	uint16_t depth; ///> number of bits per pixel, which is the color depth of the image
	uint32_t compress_type; ///> compression method being used
	uint32_t image_sz; ///> image size
	uint32_t hres; ///> horizontal resolution of the image (pixel per meter)
	uint32_t vres; ///> vertical resolution of the image (pixel per meter)
	uint32_t ncolors; ///> number of colors in the color palette
	uint32_t nimpcolors; ///>  number of important colors used
} __attribute__((packed)) BitmapInfoHeader;

/**
 * @brief bitmap image struct
 */
typedef struct
{
	BitmapFileHeader bitmapFileHeader;
	BitmapInfoHeader bitmapInfoHeader;
	short* bitmapData;
} __attribute__((packed)) bitmap_t;

/**
 * @brief reads a bitmap image to memory
 * @param filename absolute path to the bitmap file
 */
bitmap_t* bitmap_read(const char* filename);

/**
 * @brief reads a bitmap image directly to a graphics buffer
 * @param filename absolute path to the bitmap file
 */
short* bitmap_copy(const char* filename);

/**
 * @brief draws a bitmap image
 * @param bitmap instance of bitmap_t data structure
 * @param buffer graphics buffer where the image is drawn
 * @param x bitmap position on the x axis (horizontal)
 * @param y bitmap position on the y axis (vertical)
 */
void bitmap_draw(bitmap_t* bitmap, short* buffer, unsigned int x, unsigned int y);

/**
 * @brief destroys and frees memory allocated by a bitmap_t instance
 * @param bmp instance of bitmap_t data structure
 */
void bitmap_destroy(bitmap_t* bmp);

/**
 * @brief get bitmap image size (width)
 * @param bmp instance of bitmap_t data structure
 * @return returns bitmap image width (in pixels)
 */
unsigned bitmap_width(bitmap_t* bmp);

/**
 * @brief get bitmap image size (height)
 * @param bmp instance of bitmap_t data structure
 * @return returns bitmap image height (in pixels)
 */
unsigned bitmap_height(bitmap_t* bmp);

#endif /* __BITMAP_H */
