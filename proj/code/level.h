#ifndef __LEVEL_H
#define __LEVEL_H

#include "input.h"
#include "sprite.h"
#include "video.h"

typedef enum {
	PLAYING, PLAYER1_WON, PLAYER2_WON, GAME_OVER, GAME_PAUSED, EXIT_MENU, WAITING_PLAYER
} game_s;

typedef enum {
	SINGLE, SPLIT_LEFT, SPLIT_RIGHT
} pfield_t;


#define clamp(num, min, max) ((num < min) ? min : ((num > max) ? max : num))
#define make_rgb565(r, g, b)  ((short)(b >> 3) | (short) ((g >> 2) << 5) | (short) ((r >> 3) << 11))
#define midpoint(a,b) ((a + b) / 2)
#define random_between(a,b) ((int) a + (rand() % (int) (b-a)))

#define GREY_BLOCK make_rgb565(144, 144, 144)
#define BLUE_BLOCK make_rgb565(0, 140, 180)
#define RED_BLOCK make_rgb565(192, 16, 0)
#define FEUP_BLOCK make_rgb565(140, 44, 24)
#define WHITE_BLOCK make_rgb565(255, 255, 255)
#define YELLOW_BLOCK make_rgb565(248, 168, 0)
#define GREEN_BLOCK make_rgb565(64, 180, 0)
#define PURPLE_BLOCK make_rgb565(200, 0, 224)

typedef struct
{
	int x;
	int y;
	int width;
	int height;
	int health;
	int points;
	rectangle_t* shape;
	short color;
} Block;

typedef struct {

	sprite_t* ball;
	int ball_collision;

	Block* blocks;

	mouse_t* paddle;

	rectangle_t* bounds;
	rectangle_t* block_bounds;

	int lives;
	int score;

	int wall_x;
	int margin_left;
	int margin_right;

	int wall_y;
	int margin_top;
	int margin_bottom;

	int size_x;
	int size_y;

	int num_rows;
	int num_columns;
	int num_blocks;

	int block_width;
	int block_height;

} playfield_t;

typedef struct {

	playfield_t* player;

	sprite_t* lives_bitmap;

	font_t* level_font;
	font_t* score_font;

	short* background;
	short* active_buffer;
	short* blocks_buffer;

	int draw_active;
	int draw_blocks;
	int draw_name;
	int id;
	int done;

	key_t scancode;

	game_s state;

} level_t;

level_t* level_create(const char* filename, int number, int lives, int points);
void level_destroy(level_t* level);
void level_draw(level_t* level);
void level_update(level_t* level);
void level_done(level_t* level);
void level_start(playfield_t* level);
void level_reset(playfield_t* level);

playfield_t* playfield_create(int lives, int score, pfield_t type);
void playfield_destroy(playfield_t* playfield);

Block block_create(int x, int y, int width, int height, short color);
void block_destroy(level_t* level, int blockIndex);
int block_hit(playfield_t* playfield, int blockndex);
void block_draw(playfield_t* playfield, short* buffer);

#endif
