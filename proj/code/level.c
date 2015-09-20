#include "level.h"

#include <minix/syslib.h>
#include <minix/drivers.h>

#include "rtc.h"
#include "score.h"

#define sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 0))

level_t* level_create(const char* filename, int id, int lives, int score) {

	level_t* new_level = (level_t*) malloc(sizeof(level_t));

	new_level->background = bitmap_copy(filename);

	new_level->blocks_buffer = (short*) buffer_create();
	new_level->active_buffer = (short*) buffer_create();

	new_level->player = (playfield_t*) playfield_create(lives, score, SINGLE);

	const char* levelfont_filename = "/home/lcom/arkanix/res/Fipps-12.fnt";
	new_level->level_font = (font_t*) font_unpack(levelfont_filename);

	const char* scorefont_filename = "/home/lcom/arkanix/res/Hydrant-36.fnt";
	new_level->score_font = (font_t*) font_unpack(scorefont_filename);

	const char* lives_filename = "/home/lcom/arkanix/res/lives-0x117.bmp";
	new_level->lives_bitmap = sprite_create(lives_filename, 0, 0);

	new_level->lives_bitmap->xspeed = 0;
	new_level->lives_bitmap->yspeed = 0;

	new_level->draw_blocks = 1;
	new_level->draw_active = 1;
	new_level->draw_name = 1;

	new_level->done = 0;
	new_level->id = id;
	new_level->state = PLAYING;

	level_reset(new_level->player);

	return new_level;
}

void level_destroy(level_t* level)
{
	if (level == NULL)
	{
		return;
	}

	playfield_destroy(level->player);

	sprite_destroy(level->lives_bitmap);

	font_destroy(level->score_font);
	font_destroy(level->level_font);

	buffer_destroy(level->background);
	buffer_destroy(level->blocks_buffer);
	buffer_destroy(level->active_buffer);

	free(level);

	level = NULL;
}

void playfield_destroy(playfield_t* playfield)
{
	if (playfield == NULL)
	{
		return;
	}

	sprite_destroy(playfield->ball);

	mouse_destroy(playfield->paddle);

	free(playfield->bounds);
	free(playfield->block_bounds);
	free(playfield->blocks);
	free(playfield);

	playfield = NULL;
}

playfield_t* playfield_create(int lives, int score, pfield_t type)
{
	playfield_t* new_playfield = (playfield_t*) malloc(sizeof(playfield_t));

	switch (type)
	{
	case SINGLE:

		new_playfield->size_x = 572;
		new_playfield->size_y = 736;
		new_playfield->wall_x = 70;
		new_playfield->wall_y = 132;
		new_playfield->num_rows = 7;
		new_playfield->num_columns = 9;
		new_playfield->margin_left = 20;
		new_playfield->margin_right = 572;
		break;

	case SPLIT_LEFT:

		new_playfield->size_x = 435;
		new_playfield->size_y = 638;
		new_playfield->wall_x = 40;
		new_playfield->wall_y = 132;
		new_playfield->num_rows = 6;
		new_playfield->num_columns = 7;
		new_playfield->margin_left = 14;
		new_playfield->margin_right = 435;
		break;

	case SPLIT_RIGHT:

		new_playfield->size_x = 435;
		new_playfield->size_y = 638;
		new_playfield->wall_x = 550;
		new_playfield->wall_y = 132;
		new_playfield->num_rows = 6;
		new_playfield->num_columns = 7;
		new_playfield->margin_left = 14;
		new_playfield->margin_right = 435;
		break;
	}

	new_playfield->margin_top = 40;
	new_playfield->margin_bottom = 360;

	// sets bounds for playfield area
	new_playfield->bounds = (rectangle_t*) create_rectangle(
			new_playfield->wall_x, new_playfield->wall_y,
			new_playfield->wall_x + new_playfield->size_x,
			new_playfield->wall_y + new_playfield->size_y);

	// sets bounds for block area
	new_playfield->block_bounds = (rectangle_t*) create_rectangle(
			new_playfield->wall_x + new_playfield->margin_left,
			new_playfield->wall_y + new_playfield->margin_top,
			new_playfield->wall_x + new_playfield->margin_right,
			new_playfield->wall_y + new_playfield->margin_bottom);

	// sets the game lives and score
	new_playfield->lives = lives;
	new_playfield->score = score;

	// sets default size for blocks
	new_playfield->num_blocks = 0;
	new_playfield->block_height = 30;
	new_playfield->block_width = 55;

	// initializes the block array
	const int block_size = new_playfield->block_height*new_playfield->block_width;
	new_playfield->blocks = (Block*) calloc(block_size, sizeof(Block));

	// initializes the paddle (mouse cursor);
	const char* paddle_filename = "/home/lcom/arkanix/res/paddle-0x117.bmp";
	const int playfield_middle = midpoint(new_playfield->bounds->xi, new_playfield->bounds->xf);
	new_playfield->paddle = (mouse_t*) mouse_create(paddle_filename, PADDLE, playfield_middle, 700);

	// initializes the ball sprite
	const char* ball_filename = "/home/lcom/arkanix/res/ball-0x117.bmp";
	new_playfield->ball = (sprite_t*) sprite_create(ball_filename, 0, 0);

	return new_playfield;
}

void level_start(playfield_t* playfield)
{
	playfield->ball->y -= 16;

	if (playfield->paddle->xspeed <= 0)
	{
		playfield->ball->xspeed = clamp(playfield->paddle->xspeed, -10, -5);
	}
	else
	{
		playfield->ball->xspeed = clamp(playfield->paddle->xspeed, 5, 10);
	}

	playfield->ball->yspeed = -6;
	playfield->ball->moving = 1;
}

void level_reset(playfield_t* playfield)
{
	int paddle_center = midpoint(playfield->paddle->x, playfield->paddle->x + playfield->paddle->width);

	playfield->ball->x = paddle_center - (playfield->ball->width / 2);
	playfield->ball->y = playfield->paddle->y - playfield->ball->height;
	playfield->ball->xspeed = 0;
	playfield->ball->yspeed = 0;
	playfield->ball->moving = 0;
}

Block block_create(int x, int y, int width, int height, short color)
{
	Block new_block;

	new_block.x = x;
	new_block.y = y;
	new_block.color = color;
	new_block.width = width;
	new_block.height = height;

	new_block.shape = (rectangle_t*) create_rectangle(new_block.x, new_block.y,
			new_block.x + new_block.width, new_block.y + new_block.height);

	if (color == GREY_BLOCK)
	{
		new_block.health = 3;
		new_block.points = 1000;
	}
	else if (color == YELLOW_BLOCK || color == BLUE_BLOCK)
	{
		new_block.health = 2;
		new_block.points = 300;
	}
	else if (color == PURPLE_BLOCK || color == GREEN_BLOCK)
	{
		new_block.health = 1;
		new_block.points = 100;
	}
	else if (color == RED_BLOCK)
	{
		new_block.health = 1;
		new_block.points = 50;
	}
	else if (color == FEUP_BLOCK)
	{
		new_block.health = 2;
		new_block.points = 500;
	}
	else if (color == WHITE_BLOCK)
	{
		new_block.health = 1;
		new_block.points = 250;
	}
	else
	{
		new_block.health = 0;
		new_block.points = 0;
	}

	return new_block;
}

void block_draw(playfield_t* playfield, short* buffer)
{
	int i;

	for (i = 0; i < playfield->num_columns * playfield->num_rows; i++)
	{
		if (playfield->blocks[i].health > 0)
		{
			draw_rectangle_fill(playfield->blocks[i].shape, buffer, playfield->blocks[i].color);
			draw_rectangle(playfield->blocks[i].shape, buffer, 0);
		}
	}
}

void level_draw_lives(level_t* level, short* buffer)
{
	int lives_x = 50;
	int lives_y = 70;

	int i;

	for (i = 0; i < level->player->lives; i++)
	{
		sprite_position(level->lives_bitmap, lives_x, lives_y);
		sprite_draw(level->lives_bitmap, buffer);
		lives_x += level->lives_bitmap->width;
	}
}

void level_draw_scores(level_t* level)
{
	int x = 702;
	int y = 380;

	int i;

	for (i = 0; i < score_size(); i++)
	{
		char s[8];
		table_t* hs = (table_t*) score_get();

		write_string(level->level_font, x, y, hs->table[i].name, level->blocks_buffer);

		sprintf(s, "%02u=%02u=%02u", hs->table[i].date.day, hs->table[i].date.month, hs->table[i].date.year);

		write_string(level->level_font, x + 60, y, s, level->blocks_buffer);
		write_number(level->level_font, x + 180, y, hs->table[i].points, 6, level->blocks_buffer);

		y += 32;
	}
}

void level_draw(level_t* level)
{
	if (level->draw_blocks)
	{
		buffer_copy(level->background, level->blocks_buffer);
		block_draw(level->player, level->blocks_buffer);
		write_number(level->score_font, 764, 256, level->player->score, 6, level->blocks_buffer);
		level_draw_lives(level, level->blocks_buffer);
		level_draw_scores(level);
		level->draw_blocks = 0;
	}

	if (level->draw_name)
	{
		char level_name[8];
		sprintf(level_name, "LEVEL %01d", level->id);
		write_string(level->level_font, 320, 440, level_name, level->blocks_buffer);
	}

	buffer_copy(level->blocks_buffer, level->active_buffer);
	mouse_draw(level->player->paddle, level->active_buffer);
	sprite_draw(level->player->ball, level->active_buffer);

	buffer_render(level->active_buffer);
}

__inline int ball_inside(sprite_t* ball, const rectangle_t* rect)
{
	return (rect->xi <= ball->x && ball->x <= rect->xf && rect->yi <= ball->y && ball->y <= rect->yf);
}

int ball_collision_wall(playfield_t* playfield)
{
	int ball_right = playfield->bounds->xf - playfield->ball->width;
	int ball_bottom = playfield->bounds->yf - playfield->ball->height;

	// |		|	|		|
	// |o		|	|	   o|
	// |		|	|		|
	// |		|	|		|
	// |		|	|		|
	// |		|	|		|

	if (!playfield->ball_collision &&
			(playfield->ball->x + abs(playfield->ball->xspeed) < playfield->bounds->xi ||
					ball_right - playfield->ball->x < abs(playfield->ball->xspeed)))
	{
		playfield->ball->xspeed = -playfield->ball->xspeed;
		playfield->ball_collision = 1;
		return 0;
	}

	// |	o	|
	// |	    |
	// |		|
	// |		|
	// |		|
	// |		|

	if (!playfield->ball_collision && (playfield->ball->y - playfield->bounds->yi) < abs(playfield->ball->yspeed))
	{
		playfield->ball->yspeed = -playfield->ball->yspeed;
		playfield->ball_collision = 1;
		return 0;
	}

	// |		|
	// |	    |
	// |		|
	// |		|
	// |		|
	// |    o	|

	if (abs(playfield->ball->y - ball_bottom) < 5)
	{
		if (playfield->lives > 0)
		{
			playfield->lives--;
		}
		level_reset(playfield);
		return 1;
	}

	playfield->ball_collision = 0;
	return 0;
}

void ball_collision_paddle(playfield_t* playfield) {

	const rectangle_t* paddle_bounds = (rectangle_t*) create_rectangle(
			playfield->paddle->x, playfield->paddle->y,
			playfield->paddle->x + playfield->paddle->width,
			playfield->paddle->y + playfield->paddle->height);

	const int32_t ball_center = midpoint(playfield->ball->x,
			playfield->ball->x + playfield->ball->width);
	const int32_t ball_center_y = midpoint(playfield->ball->y,
			playfield->ball->y + playfield->ball->height);
	const int32_t paddle_center = midpoint(playfield->paddle->x,
			playfield->paddle->x + playfield->paddle->width);
	const int32_t paddle_center_y = midpoint(playfield->paddle->y,
			playfield->paddle->y + playfield->paddle->height);

	const int32_t overlap_top = abs(playfield->ball->y + playfield->ball->height					- playfield->paddle->y);
	const int32_t overlap_left = (playfield->ball->x + playfield->ball->width) - paddle_bounds->xi;
	const int32_t overlap_right = playfield->ball->x - paddle_bounds->xf;

	if (overlap_top < playfield->ball->yspeed && !playfield->ball_collision
			&& ball_center >= paddle_bounds->xi
			&& ball_center <= paddle_bounds->xf)
	{
		const int32_t critical_zone = playfield->paddle->width / 6;

		if (abs(playfield->paddle->xspeed) <= 2)
		{
			playfield->ball->yspeed = -playfield->ball->yspeed;
		}
		else
		{
			playfield->ball->yspeed = -playfield->ball->yspeed;

			if (ball_center > paddle_center - critical_zone && ball_center <= paddle_center)
			{
				playfield->ball->xspeed = -sign(playfield->ball->xspeed)*clamp(abs(playfield->paddle->xspeed), 4, 5);
			}
			else if (ball_center <= paddle_center + critical_zone && ball_center > paddle_center)
			{
				playfield->ball->xspeed = -sign(playfield->ball->xspeed)*clamp(abs(playfield->paddle->xspeed), 4, 5);
			}
			else
			{
				if (playfield->paddle->xspeed >= 0)
				{
					playfield->ball->xspeed = clamp(abs(playfield->paddle->xspeed/4), 6, 9);
				}
				else
				{
					playfield->ball->xspeed = -clamp(abs(playfield->paddle->xspeed/4), 6, 9);
				}
			}
		}
	}
	else if ((overlap_left > -5 && overlap_right < 5)
			&& !playfield->ball_collision && ball_center_y > paddle_bounds->yi && ball_center_y < paddle_bounds->yf)
	{
		playfield->ball->xspeed = -playfield->ball->xspeed;

		if (ball_center_y < paddle_center_y)
		{
			playfield->ball->yspeed = -playfield->ball->yspeed;
		}
	}
	else
	{

	}
	if (ball_inside(playfield->ball, paddle_bounds))
	{
		if (playfield->ball->moving)
		{
			playfield->ball_collision = 1;
		}
		else
		{
			playfield->ball_collision = 0;
		}
	}
	else
	{
		playfield->ball_collision = 0;
	}
}

int ball_collision_block(playfield_t* playfield, int blockIndex)
{
	if (playfield->blocks[blockIndex].health <= 0)
	{
		return 0;
	}

	int ball_xf = playfield->ball->x + playfield->ball->width;
	int ball_yf = playfield->ball->y + playfield->ball->height;

	Block current_block = playfield->blocks[blockIndex];

	const rectangle_t* hitbox = (rectangle_t*) create_rectangle(current_block.x,
			current_block.y, current_block.x + playfield->block_width,
			current_block.y + playfield->block_height);

	if (!playfield->ball_collision && ball_inside(playfield->ball, hitbox))
	{
		const int overlapLeft = ball_xf - hitbox->xi - playfield->ball->xspeed;
		const int overlapRight = hitbox->xf - playfield->ball->x + playfield->ball->xspeed;
		const int overlapTop = ball_yf - hitbox->yi - playfield->ball->yspeed;
		const int overlapBottom = hitbox->yf - playfield->ball->y + playfield->ball->yspeed;

		// checks if ball is approaching from the left (vs right)
		int ballFromLeft = (abs(overlapLeft) < abs(overlapRight)) ? 1 : 0;
		int overlap_x = ballFromLeft ? overlapLeft : overlapRight;

		// checks if ball is approaching from the top (vs bottom)
		int ballFromTop = (abs(overlapTop) < abs(overlapBottom)) ? 1 : 0;
		int overlap_y = ballFromTop ? overlapTop : overlapBottom;

		if (abs(overlap_x) < abs(overlap_y))
		{
			playfield->ball->xspeed = -playfield->ball->xspeed;
		}
		else
		{
			playfield->ball->yspeed = -playfield->ball->yspeed;
		}

		playfield->ball_collision = 1;
		return 1;
	}
	else
	{
		return 0;
	}

}

void level_update(level_t* level)
{
	if (!level->player->ball->moving)
	{
		level->draw_name = 1;
		if (level->player->paddle->leftButton)
		{
			level->draw_name = 0;
			level->draw_blocks = 1;
			level_start(level->player);
			return;
		}
		int paddle_center = midpoint(level->player->paddle->x, level->player->paddle->x + level->player->paddle->width);
		level->player->ball->x = paddle_center - (level->player->ball->width / 2);
	}

	sprite_update(level->player->ball);
	ball_collision_paddle(level->player);

	if (ball_collision_wall(level->player))
	{
		level->draw_blocks = 1;
	}

	if (ball_inside(level->player->ball, level->player->block_bounds))
	{
		int i;

		for (i = 0; i < level->player->num_rows * level->player->num_columns; i++)
		{
			if (ball_collision_block(level->player, i))
			{
				if (block_hit(level->player, i))
				{
					level->draw_blocks = 1;
				}
				break;
			}
		}
	}

	if (level->player->paddle->rightButton)
	{
		level->state = EXIT_MENU;
		level->done = 1;
		return;
	}

	level_draw(level);
	level_done(level);
}

void level_done(level_t* level)
{
	if (level->state == PLAYING)
	{
		if (level->scancode == KEY_ESC)
		{
			level->done = 1;
			level->state = EXIT_MENU;
		}
		if (level->player->lives <= 0)
		{
			level->done = 1;
			level->state = GAME_OVER;
		}
		else if (level->player->num_blocks <= 0)
		{
			level->done = 1;
			level->state = PLAYER1_WON;
		}
	}
}

int block_hit(playfield_t* playfield, int block_index)
{
	if (playfield->blocks[block_index].health > 0)
	{
		playfield->blocks[block_index].health--;
		if (playfield->blocks[block_index].health == 0)
		{
			playfield->num_blocks--;
		}
		if (playfield->blocks[block_index].color == GREY_BLOCK)
		{
			playfield->blocks[block_index].color = YELLOW_BLOCK;
		}
		else if (playfield->blocks[block_index].color == YELLOW_BLOCK)
		{
			playfield->blocks[block_index].color = GREEN_BLOCK;
		}
		else if (playfield->blocks[block_index].color == BLUE_BLOCK)
		{
			playfield->blocks[block_index].color = PURPLE_BLOCK;
		}
		else if (playfield->blocks[block_index].color == FEUP_BLOCK)
		{
			playfield->blocks[block_index].color = WHITE_BLOCK;
		}
		playfield->score += playfield->blocks[block_index].points;
		return 1;
	}
	return 0;
}
