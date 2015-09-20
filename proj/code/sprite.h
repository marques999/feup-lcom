#ifndef __SPRITE_H
#define __SPRITE_H

#include "bitmap.h"

typedef struct {

	bitmap_t* bitmap;

	int x; // current sprite position (horizontal)
	int y; // current sprite position (vertical)
	int width; // sprite dimensions (x)
	int height; // sprite dimensions (y)
	int moving;
	int xspeed; // current speed on x axis
	int yspeed; // current speed on y axis

} sprite_t;

sprite_t* sprite_create(const char* filename, int x, int y);
void sprite_destroy(sprite_t* sprite);
void sprite_draw(sprite_t* sprite, short* buffer);
void sprite_position(sprite_t* sprite, int x, int y);
void sprite_update(sprite_t* sprite);

#endif /* __SPRITE_H */
