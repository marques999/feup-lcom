#include <minix/syslib.h>
#include <minix/drivers.h>

#include "sprite.h"
#include "video_gr.h"

void sprite_draw(Sprite* sp) {
	vg_draw_xpm(sp->x, sp->y, sp->width, sp->height, sp->map);
}

Sprite* sprite_create(char* xpm[]) {
	Sprite *sp = (Sprite*) malloc(sizeof(Sprite));
	if (sp == NULL) {
		return NULL;
	}
	sp->map = (char*) vg_read_xpm(xpm, &(sp->width), &(sp->height));
	if (sp->map == NULL) {
		free(sp);
		return NULL;
	}
	return sp;
}

void sprite_destroy(Sprite* sp) {
	if (sp == NULL) {
		return;
	}
	free(sp->map);
	free(sp);
	sp = NULL;
}

void sprite_move(Sprite* sp) {
	sp->x += sp->xspeed;
	sp->y += sp->yspeed;
	sprite_draw(sp);
}
