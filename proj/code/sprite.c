#include "sprite.h"

sprite_t* sprite_create(const char* filename, int x, int y) {

	sprite_t* new_sprite = (sprite_t*) malloc(sizeof(sprite_t));

	new_sprite->bitmap = (bitmap_t*) bitmap_read(filename);

	if (new_sprite->bitmap == NULL) {
		free(new_sprite);
		return NULL;
	}

	new_sprite->x = x;
	new_sprite->y = y;
	new_sprite->moving = 0;
	new_sprite->width = bitmap_width(new_sprite->bitmap);
	new_sprite->height = bitmap_height(new_sprite->bitmap);

	return new_sprite;
}

__inline void sprite_position(sprite_t* sprite, int x, int y) {

	sprite->x = x;
	sprite->y = y;
}

__inline void sprite_update(sprite_t* sprite) {

	sprite->x += sprite->xspeed;
	sprite->y += sprite->yspeed;
}

void sprite_draw(sprite_t* sprite, short* buffer) {

	if (sprite->bitmap == NULL) {
		return;
	}

	int x, y;
	int i = 0;

	for (y = sprite->y + sprite->height - 1; y >= sprite->y; y--) {
		for (x = sprite->x; x < sprite->x + sprite->width; x++) {
			if (sprite->bitmap->bitmapData[i] != (short) 0xF81F) {
				draw_pixel(x, y, sprite->bitmap->bitmapData[i], buffer);
			}
			i++;
		}
		if (sprite->width % 2) {
			i++;
		}
	}
}

void sprite_destroy(sprite_t* sprite) {

	if (sprite == NULL) {
		return;
	}

	bitmap_destroy(sprite->bitmap);

	free(sprite);

	sprite = NULL;
}
