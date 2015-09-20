#ifndef _SS_H
#define _SS_H

#include "level.h"
#include "video.h"
#include "serial.h"

typedef struct {

	playfield_t* p1;
	playfield_t* p2;

	sprite_t* lives_bitmap;

	font_t* level_font;
	font_t* score_font;

	int p1_reset;
	int p2_reset;
	int p1_quit;
	int p2_quit;

	bitmap_t* gamedraw_bitmap;
	bitmap_t* victory_bitmap;

	int start;

	short* background;
	short* active_buffer;
	short* blocks_buffer;

	int draw_active;
	int draw_blocks;

	int p2_ready;

	int done;

	game_s state;

	int time;

} ss_t;

ss_t* ss_create();
void ss_destroy(ss_t* splitscren);
void ss_start(ss_t* splitscreen);
void ss_reset(ss_t* splitscreen);
void ss_draw(ss_t* splitscreen);
void ss_draw_lives(ss_t* splitscreen, short* buffer);
void ss_check(ss_t* splitscreen);
void ss_update(ss_t* splitscreen);

#endif /* __SS_H */
