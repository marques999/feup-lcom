#include "ss.h"

ss_t* ss_create(int s)
{
	ss_t* new_splitscreen = (ss_t*) malloc(sizeof(ss_t));

	const char* ssbg_filename = "/home/lcom/arkanix/res/level0-0x117.bmp";
	new_splitscreen->background = bitmap_copy(ssbg_filename);

	new_splitscreen->blocks_buffer = (short*) buffer_create();
	new_splitscreen->active_buffer = (short*) buffer_create();

	new_splitscreen->p1 = (playfield_t*) playfield_create(3, 0, SPLIT_LEFT);
	new_splitscreen->p2 = (playfield_t*) playfield_create(3, 0, SPLIT_RIGHT);

	const char* scorefont_filename = "/home/lcom/arkanix/res/Hydrant-36.fnt";
	new_splitscreen->score_font = (font_t*) font_unpack(scorefont_filename);

	const char* levelfont_filename = "/home/lcom/arkanix/res/Fipps-12.fnt";
	new_splitscreen->level_font = (font_t*) font_unpack(levelfont_filename);

	const char* lives_filename = "/home/lcom/arkanix/res/lives-0x117.bmp";
	new_splitscreen->lives_bitmap = sprite_create(lives_filename, 0, 0);
	new_splitscreen->lives_bitmap->xspeed = 0;
	new_splitscreen->lives_bitmap->yspeed = 0;

	const char* gamedraw_filename = "/home/lcom/arkanix/res/rubberduck-0x117.bmp";
	new_splitscreen->gamedraw_bitmap = (bitmap_t*) bitmap_read(gamedraw_filename);

	const char* victory_filename = "/home/lcom/arkanix/res/yourwinner-0x117.bmp";
	new_splitscreen->victory_bitmap = (bitmap_t*) bitmap_read(victory_filename);

	new_splitscreen->p1_reset = 1;
	new_splitscreen->p2_reset = 1;
	new_splitscreen->p2_ready = 0;

	new_splitscreen->p1_quit = 0;
	new_splitscreen->p2_quit = 0;

	new_splitscreen->done = 0;
	new_splitscreen->state = s ? WAITING_PLAYER : PLAYING;

	new_splitscreen->time = 60;
	new_splitscreen->start = 0;

	new_splitscreen->draw_active = 1;
	new_splitscreen->draw_blocks = 1;

	ss_reset(new_splitscreen);

	return new_splitscreen;
}

void ss_destroy(ss_t* splitscreen)
{
	if (splitscreen == NULL)
	{
		return;
	}

	playfield_destroy(splitscreen->p1);
	playfield_destroy(splitscreen->p2);

	sprite_destroy(splitscreen->lives_bitmap);

	buffer_destroy(splitscreen->background);
	buffer_destroy(splitscreen->blocks_buffer);
	buffer_destroy(splitscreen->active_buffer);

	font_destroy(splitscreen->score_font);
	font_destroy(splitscreen->level_font);

	free(splitscreen);

	splitscreen = NULL;
}

void ss_start(ss_t* splitscreen)
{
	if (!splitscreen->p1->ball->moving)
	{
		splitscreen->draw_blocks = 1;

		if (splitscreen->p1->paddle->leftButton)
		{
			if (!splitscreen->start)
			{
				splitscreen->start = 1;
			}

			level_start(splitscreen->p1);

			return;
		}

		int paddle_center = midpoint(splitscreen->p1->paddle->x, splitscreen->p1->paddle->x + splitscreen->p1->paddle->width);
		splitscreen->p1->ball->x = paddle_center - (splitscreen->p1->ball->width / 2);
	}

	if (!splitscreen->p2->ball->moving)
	{
		splitscreen->draw_blocks = 1;

		if (splitscreen->p2->paddle->leftButton)
		{
			if (!splitscreen->start)
			{
				splitscreen->start = 1;
			}

			level_start(splitscreen->p2);

			return;
		}

		int paddle_center = midpoint(splitscreen->p2->paddle->x, splitscreen->p2->paddle->x + splitscreen->p2->paddle->width);
		splitscreen->p2->ball->x = paddle_center - (splitscreen->p2->ball->width / 2);
	}
}

void ss_reset(ss_t* splitscreen)
{
	if (splitscreen->p1_reset)
	{
		level_reset(splitscreen->p1);
		splitscreen->p1_reset = 0;
	}

	if (splitscreen->p2_reset)
	{
		level_reset(splitscreen->p2);
		splitscreen->p2_reset = 0;
	}
}

void ss_draw_lives(ss_t* splitscreen, short* buffer)
{
	int lives_x;
	int lives_y;
	int i;

	lives_x = 335;
	lives_y = 70;

	for (i = 0; i < splitscreen->p1->lives; i++)
	{
		sprite_position(splitscreen->lives_bitmap, lives_x, lives_y);
		sprite_draw(splitscreen->lives_bitmap, buffer);
		lives_x += splitscreen->lives_bitmap->width;
	}

	lives_x = 630;
	lives_y = 70;

	for (i = 0; i < splitscreen->p2->lives; i++)
	{
		sprite_position(splitscreen->lives_bitmap, lives_x, lives_y);
		sprite_draw(splitscreen->lives_bitmap, buffer);
		lives_x += splitscreen->lives_bitmap->width;
	}
}

void ss_draw(ss_t* splitscreen)
{
	if (splitscreen->draw_blocks)
	{
		buffer_copy(splitscreen->background, splitscreen->blocks_buffer);

		block_draw(splitscreen->p1, splitscreen->blocks_buffer);
		block_draw(splitscreen->p2, splitscreen->blocks_buffer);

		write_number(splitscreen->score_font, 26, 60, splitscreen->p1->score, 6, splitscreen->blocks_buffer);
		write_number(splitscreen->score_font, 845, 60, splitscreen->p2->score, 6, splitscreen->blocks_buffer);

		ss_draw_lives(splitscreen, splitscreen->blocks_buffer);
	}

	switch (splitscreen->state)
	{

	case PLAYING:

		if (splitscreen->draw_active)
		{
			buffer_copy(splitscreen->blocks_buffer, splitscreen->active_buffer);

			mouse_draw(splitscreen->p1->paddle, splitscreen->active_buffer);
			mouse_draw(splitscreen->p2->paddle, splitscreen->active_buffer);

			write_number(splitscreen->score_font, 492, 60, splitscreen->time, 2, splitscreen->active_buffer);

			sprite_draw(splitscreen->p1->ball, splitscreen->active_buffer);
			sprite_draw(splitscreen->p2->ball, splitscreen->active_buffer);
		}

		break;

	case PLAYER1_WON:

		if (splitscreen->draw_blocks)
		{
			short* new_buffer = (short*) buffer_create();

			bitmap_draw(splitscreen->victory_bitmap, new_buffer, 387, 200);
			buffer_blend(new_buffer, splitscreen->blocks_buffer, 64);

			if (splitscreen->p2_quit)
			{
				write_string(splitscreen->level_font, 360, 456, "PLAYER 2 DISCONNECTED", splitscreen->blocks_buffer);
			}
			else
			{
				write_string(splitscreen->level_font, 424, 456, "PLAYER 1 WINS", splitscreen->blocks_buffer);
			}

			write_number(splitscreen->score_font, 437, 490, splitscreen->p1->score, 6, splitscreen->blocks_buffer);
			buffer_copy(splitscreen->blocks_buffer, splitscreen->active_buffer);
		}

		break;

	case PLAYER2_WON:

		if (splitscreen->draw_blocks)
		{
			short* new_buffer = (short*) buffer_create();

			bitmap_draw(splitscreen->victory_bitmap, new_buffer, 387, 200);
			buffer_blend(new_buffer, splitscreen->blocks_buffer, 64);

			if (splitscreen->p1_quit)
			{
				write_string(splitscreen->level_font, 360, 456, "PLAYER 1 DISCONNECTED", splitscreen->blocks_buffer);
			}
			else
			{
				write_string(splitscreen->level_font, 424, 456, "PLAYER 2 WINS", splitscreen->blocks_buffer);
			}

			write_number(splitscreen->score_font, 437, 490, splitscreen->p2->score, 6, splitscreen->blocks_buffer);
			buffer_copy(splitscreen->blocks_buffer, splitscreen->active_buffer);
		}

		break;

	case GAME_OVER:

		if (splitscreen->draw_blocks)
		{
			short* new_buffer = (short*) buffer_create();

			bitmap_draw(splitscreen->gamedraw_bitmap, new_buffer, 387, 200);

			buffer_blend(new_buffer, splitscreen->blocks_buffer, 64);

			write_string(splitscreen->level_font, 440, 460, "GAME DRAW", splitscreen->blocks_buffer);
			write_number(splitscreen->score_font, 340, 490, splitscreen->p1->score, 6, splitscreen->blocks_buffer);
			write_number(splitscreen->score_font, 520, 490, splitscreen->p2->score, 6, splitscreen->blocks_buffer);

			buffer_copy(splitscreen->blocks_buffer, splitscreen->active_buffer);
		}

	break;

	case WAITING_PLAYER:

		if (splitscreen->draw_blocks)
		{
			short* new_buffer = (short*) buffer_create();

			buffer_blend(new_buffer, splitscreen->blocks_buffer, 64);
			write_string(splitscreen->level_font, 380, 360, "WAITING FOR PLAYER", splitscreen->blocks_buffer);
			buffer_copy(splitscreen->blocks_buffer, splitscreen->active_buffer);
		}

		break;
	}

	splitscreen->draw_active = 0;
	splitscreen->draw_blocks = 0;

	buffer_render(splitscreen->active_buffer);
}

void ss_update(ss_t* splitscreen)
{
	if (splitscreen->state == PLAYING)
	{
		ss_start(splitscreen);

		sprite_update(splitscreen->p1->ball);
		sprite_update(splitscreen->p2->ball);

		if (splitscreen->p1->paddle->rightButton)
		{
			splitscreen->p1_quit = 1;
		}

		if (splitscreen->p2->paddle->rightButton)
		{
			splitscreen->p2_quit = 1;
		}

		if (ball_collision_paddle(splitscreen->p1))
		{
			splitscreen->draw_active = 1;
		}

		if (ball_collision_paddle(splitscreen->p2))
		{
			splitscreen->draw_active = 1;
		}

		if (ball_collision_wall(splitscreen->p1))
		{
			splitscreen->draw_active = 1;
		}

		if (ball_collision_wall(splitscreen->p2))
		{
			splitscreen->draw_active = 1;
		}

		if (ball_inside(splitscreen->p1->ball, splitscreen->p1->block_bounds))
		{
			int i;

			for (i = 0; i < splitscreen->p1->num_rows * splitscreen->p1->num_columns; i++)
			{
				if (ball_collision_block(splitscreen->p1, i))
				{
					if (block_hit(splitscreen->p1, i))
					{
						splitscreen->draw_blocks = 1;
					}

					break;
				}
			}
		}

		if (ball_inside(splitscreen->p2->ball, splitscreen->p2->block_bounds))
		{
			int i;

			for (i = 0; i < splitscreen->p2->num_rows * splitscreen->p2->num_columns; i++)
			{
				if (ball_collision_block(splitscreen->p2, i))
				{
					if (block_hit(splitscreen->p2, i))
					{
						splitscreen->draw_blocks = 1;
					}

					break;
				}
			}
		}

		if (splitscreen->p1->paddle->rightButton)
		{
			splitscreen->p1_quit = 1;
		}

		if (splitscreen->p2->paddle->rightButton)
		{
			splitscreen->p2_quit = 1;
		}
	}

	ss_draw(splitscreen);
	ss_check(splitscreen);
}

void ss_check(ss_t* splitscreen)
{

	switch (splitscreen->state)
	{
	case PLAYING:

		if (splitscreen->p1_quit)
		{
			rtc_write_alarm(5);
			splitscreen->draw_blocks = 1;
			splitscreen->state = PLAYER2_WON;

			return;
		}

		if (splitscreen->p2_quit)
		{
			rtc_write_alarm(5);
			splitscreen->draw_blocks = 1;
			splitscreen->state = PLAYER1_WON;

			return;
		}

		if (splitscreen->p2->lives <= 0 || splitscreen->p1->num_blocks <= 0)
		{
			rtc_write_alarm(5);
			splitscreen->draw_blocks = 1;
			splitscreen->state = PLAYER1_WON;

			return;
		}

		if (splitscreen->p1->lives <= 0 || splitscreen->p2->num_blocks <= 0)
		{
			rtc_write_alarm(5);
			splitscreen->draw_blocks = 1;
			splitscreen->state = PLAYER2_WON;

			return;
		}

		if (splitscreen->time <= 0)
		{
			rtc_write_alarm(5);

			if (splitscreen->p1->score > splitscreen->p2->score)
			{
				splitscreen->state = PLAYER1_WON;
			}
			else if (splitscreen->p1->score < splitscreen->p2->score)
			{
				splitscreen->state = PLAYER2_WON;
			}
			else
			{
				splitscreen->state = GAME_OVER;
			}

			splitscreen->draw_blocks = 1;
		}

		break;

	case WAITING_PLAYER:

		if (splitscreen->p1->paddle->rightButton)
		{
			splitscreen->done = 1;

			return;
		}

		if (splitscreen->p2_ready)
		{
			splitscreen->state = PLAYING;
		}

		break;

	case PLAYER1_WON:

		if (splitscreen->p1->paddle->leftButton || splitscreen->p2->paddle->leftButton)
		{
			splitscreen->done = 1;
		}

		break;

	case PLAYER2_WON:

		if (splitscreen->p1->paddle->leftButton || splitscreen->p2->paddle->leftButton)
		{
			splitscreen->done = 1;
		}

		break;

	case GAME_OVER:

		if (splitscreen->p1->paddle->leftButton || splitscreen->p1->paddle->leftButton)
		{
			splitscreen->done = 1;
		}

		break;

	}
}
