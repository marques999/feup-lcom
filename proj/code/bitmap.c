#include "bitmap.h"
#include "video.h"

bitmap_t* bitmap_read(const char* filename)
{
	bitmap_t* bmp = (bitmap_t*) malloc(sizeof(bitmap_t));

	FILE *fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		video_exit();
		printf("arkanix::bitmap not found %s\n", filename);
		exit(1);
	}

	BitmapFileHeader bitmapFileHeader;

	fread(&bitmapFileHeader, sizeof(bitmapFileHeader), 1, fp);

	if (bitmapFileHeader.magic != 0x4D42)
	{
		fclose(fp);
		return NULL;
	}

	BitmapInfoHeader bitmapInfoHeader;

	fread(&bitmapInfoHeader, sizeof(bitmapInfoHeader), 1, fp);
	fseek(fp, bitmapFileHeader.offset, SEEK_SET);

	short* bitmapImage = (short*) malloc(bitmapInfoHeader.image_sz);

	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(fp);
		return NULL;
	}

	fread((char*) bitmapImage, bitmapInfoHeader.image_sz, 1, fp);

	if (bitmapImage == NULL)
	{
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	bmp->bitmapData = bitmapImage;
	bmp->bitmapFileHeader = bitmapFileHeader;
	bmp->bitmapInfoHeader = bitmapInfoHeader;

	return bmp;
}

short* bitmap_copy(const char* filename)
{
	bitmap_t* bmp = (bitmap_t*) bitmap_read(filename);
	short* buffer = (short*) buffer_create();
	bitmap_draw(bmp, buffer, 0, 0);
	free(bmp);
	return ((short*) buffer);
}

void bitmap_destroy(bitmap_t *bmp)
{
	if (bmp == NULL)
	{
		return;
	}

	free(bmp->bitmapData);
	free(bmp);

	bmp = NULL;
}

void bitmap_draw(bitmap_t* bitmap, short* buffer, unsigned int x, unsigned int y)
{
	if (bitmap == NULL)
	{
		return;
	}

	int width = bitmap_width(bitmap);
	int height = bitmap_height(bitmap);

	if (!video_check_bounds(x, y))
	{
		return;
	}

	int i;
	int screen_location = y + height - 1;

	short* buffer_position = (short*) buffer + screen_location * video_width() + x;
	short* bitmap_position = bitmap->bitmapData;

	for (i = 0; i < height; i++)
	{
		memcpy((char*) buffer_position, (char*) bitmap_position, width << 1);

		if (width % 2 == 0)
		{
			bitmap_position += width;
		}
		else
		{
			bitmap_position += width + 1;
		}

		buffer_position -= video_width();
	}
}

unsigned bitmap_width(bitmap_t *bmp)
{
	return bmp->bitmapInfoHeader.width;
}

unsigned bitmap_height(bitmap_t *bmp)
{
	return bmp->bitmapInfoHeader.height;
}

