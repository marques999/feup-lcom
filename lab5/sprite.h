#ifndef __SPRITE_H
#define __SPRITE_H

typedef struct {
	int x; // current position (horizontal)
	int y; // current position (vertical)
	int width; // dimensions (x)
	int height; // dimensions (y)
	int xspeed; // current speed on x axis
	int yspeed; // current speed on y axis
	char* map;
} Sprite;

Sprite* sprite_create(char* xpm[]);
void sprite_destroy(Sprite* sp);
void sprite_move(Sprite* sp);

#endif
