#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/drivers.h>

#include "arkanix.h"
#include "level.h"
#include "menu.h"
#include "rtc.h"
#include "serial.h"
#include "ss.h"
#include "timer.h"

ArkanixState* arkanix = NULL;

/**
 * DEVICE INTERRUPT HOOKS
 *
 * Timer (timer_hook) = 0
 * Keyboard (kbd_hook) = 1
 * Mouse (mouse_hook) = 2
 * Serial (com_hook) = 3
 * RTC (rtc_hook) = 4
 *
 */

hsscreen_t* hss = NULL;

/**
 * @brief aborts game initialization, displays message to user
 */
void arkanix_abort()
{
	video_exit();
	printf("arkanix::init failed\n");
	exit(1);
}

/**
 * @brief creates a new game instance
 * - subscribes keyboard, real-time_clock, serial port and timer interrupts
 * - disables serial port interrupts for now (only needed for multiplayer!)
 * - resets keyboard scancodes for player and serial communication
 */
void arkanix_start()
{
	arkanix = (ArkanixState*) malloc(sizeof(ArkanixState));
	if (arkanix == NULL)
	{
		arkanix_abort();
	}

	arkanix->kbd_hook = kbd_subscribe();
	if (arkanix->kbd_hook == -1)
	{
		arkanix_abort();
	}

	arkanix->rtc_hook = rtc_subscribe();
	if (arkanix->rtc_hook == -1)
	{
		arkanix_abort();
	}

	arkanix->serial_hook = serial_init();
	if (arkanix->serial_hook == -1)
	{
		arkanix_abort();
	}

	arkanix->timer_hook = timer_subscribe();
	if (arkanix->timer_hook == -1)
	{
		arkanix_abort();
	}

	arkanix->done = 0;
	arkanix->kbd_scancode = 0;
	arkanix->ser_scancode = 0;
	arkanix->mode = MODE_MOUSE;
	arkanix->state = INIT;
	arkanix->statep = NULL;

	timer_init();
}

/**
 * @brief initializes the score input screen interface
 * - creates an empty graphics buffer
 * - reads image assets and sets their initial position on the screen
 */
void hss_create()
{
	hss = (hsscreen_t*) malloc(sizeof(hsscreen_t));

	hss->buffer = (short*) buffer_create();

	const char* us_filename = "/home/lcom/arkanix/res/underscore-0x117.bmp";
	hss->underscore = (sprite_t*) sprite_create(us_filename, 0, 0);

	const char* victory_filename = "/home/lcom/arkanix/res/yourwinner-0x117.bmp";
	hss->victory_bitmap = (bitmap_t*) bitmap_read(victory_filename);

	const char* levelfont_filename = "/home/lcom/arkanix/res/Fipps-12.fnt";
	hss->level_font = (font_t*) font_unpack(levelfont_filename);

	sprite_position(hss->underscore, 631, 400);

	hss->player_name = malloc(4 * sizeof(char));
	hss->player_name = "AAA";

	hss->draw = 1;
	hss->position = 0;
	hss->done = 0;
}

/**
 * @brief draws the score input screen
 * - fills graphics buffer with black color (redraw)
 * - draws bitmap, text, player name and cursor
 * - toggles the cursor periodically for a nice blinking animation
 */
void hss_draw()
{
	buffer_fill(hss->buffer);

	bitmap_draw(hss->victory_bitmap, hss->buffer, 185, 259);

	write_string(hss->level_font, 562, 300, "NEW HIGH SCORE!", hss->buffer);
	write_string(hss->level_font, 502, 330, "PLASE ENTER YOUR NAME: ", hss->buffer);

	glyph_draw(hss->level_font, hss->player_name[0] - '0', 635, 376, hss->buffer);
	glyph_draw(hss->level_font, hss->player_name[1] - '0', 655, 376, hss->buffer);
	glyph_draw(hss->level_font, hss->player_name[2] - '0', 675, 376, hss->buffer);

	write_string(hss->level_font, 478, 442, "<USE ARROW KEYS TO NAVIGATE>", hss->buffer);

	if (timer_current_frame() % 15 == 0)
	{
		hss->draw ^= 1;
	}
	if (hss->draw)
	{
		sprite_draw(hss->underscore, hss->buffer);
	}

	buffer_render(hss->buffer);
}

void hss_update()
{
	hss->done = 0;

	if (arkanix->kbd_scancode == 0xcb)
	{
		if (hss->position > 0)
		{
			hss->position--;
			hss->underscore->x -= 20;
		}
	}
	else if (arkanix->kbd_scancode == 0xcd)
	{
		if (hss->position < 2)
		{
			hss->underscore->x += 20;
			hss->position++;
		}
	}
	else if (arkanix->kbd_scancode == 0xc8)
	{
		if (hss->player_name[hss->position] == 'Z')
		{
			hss->player_name[hss->position] = 'A';
		}
		else if (hss->player_name[hss->position] >= 'A' && hss->player_name[hss->position] < 'Z')
		{
			hss->player_name[hss->position]++;
		}
	}
	else if (arkanix->kbd_scancode == 0xd0)
	{
		if (hss->player_name[hss->position] == 'A')
		{
			hss->player_name[hss->position] = 'Z';
		}
		else if (hss->player_name[hss->position] > 'A' && hss->player_name[hss->position] <= 'Z')
		{
			hss->player_name[hss->position]--;
		}
	}
	else if (arkanix->kbd_scancode == (KEY_ENTER | 0x80))
	{
		char* new_name = malloc(4);
		strcpy(new_name, hss->player_name);
		score_add(new_name, arkanix->score);
		hss->done = 1;
	}

	arkanix->kbd_scancode = 0;
}

/**
 * @brief disables and unsubscribes mouse interrupts
 */
__inline void mouse_disable()
{
	mouse_unsubscribe();
}

/**
 * @brief deletes the current game state
 * - disables RTC interrupts, and enables them back only if needed
 */
void arkanix_delete_state()
{
	mouse_disable();

	switch (arkanix->state)
	{

	case MAIN_MENU:

		menu_delete((menu_t*) arkanix->statep);

		break;

	case SINGLEPLAYER:

		level_destroy((level_t*) arkanix->statep);

		break;

	case MULTIPLAYER_LOCAL:

		ss_destroy((ss_t*) arkanix->statep);

		rtc_disable();

		break;

	case MULTIPLAYER_SERIAL:

		ss_destroy((ss_t*) arkanix->statep);

		rtc_disable();
		serial_disable();

		break;

	}
}

/**
 * @brief initializes a new singleplayer mode game
 */
void arkanix_singleplayer()
{
	const char* level1_filename = "/home/lcom/arkanix/res/level1-0x117.bmp";
	const char* level2_filename = "/home/lcom/arkanix/res/level2-0x117.bmp";
	const char* level3_filename = "/home/lcom/arkanix/res/level3-0x117.bmp";

	switch (arkanix->level)
	{

	case 1:

		arkanix->statep = (level_t*) level_create(level1_filename, arkanix->level, arkanix->lives, arkanix->score);
		generate_level1((level_t*) arkanix->statep);
		break;

	case 2:

		arkanix->statep = (level_t*) level_create(level2_filename, arkanix->level, arkanix->lives, arkanix->score);
		generate_level2((level_t*) arkanix->statep);
		break;

	case 3:

		arkanix->statep = (level_t*) level_create(level3_filename, arkanix->level, arkanix->lives, arkanix->score);
		generate_level3((level_t*) arkanix->statep);
		break;

	}
}

/**
 * @brief initializes a multiplayer game (local or serial)
 */
__inline void arkanix_multiplayer(int s)
{
	arkanix->statep = (ss_t*) ss_create(s);
	ss_generate((ss_t*) arkanix->statep);
}

/**
 * @brief manages transistions between states
 */
void arkanix_change_state(GameState new_state)
{
	arkanix_delete_state();
	arkanix->state = new_state;

	mouse_disable();

	switch (arkanix->state)
	{

	case MAIN_MENU:

		arkanix->statep = (menu_t*) menu_create();
		arkanix->level = 1;
		arkanix->score = 0;
		arkanix->lives = 3;
		arkanix->mode = MODE_MOUSE;
		arkanix->mouse_hook = mouse_subscribe();
		break;

	case SINGLEPLAYER:

		if (arkanix->mode == MODE_MOUSE)
		{
			arkanix->mouse_hook = mouse_subscribe();
		}

		arkanix_singleplayer();
		break;

	case MULTIPLAYER_LOCAL:

		rtc_enable();
		arkanix->mouse_hook = mouse_subscribe();
		arkanix_multiplayer(0);
		break;

	case MULTIPLAYER_SERIAL:

		rtc_enable();
		serial_enable();
		serial_write_char(0x55);
		arkanix_multiplayer(1);
		break;

	case SCORE_SCREEN:

		if (hss == NULL)
		{
			hss_create();
		}

		break;

	}
}

/**
 * @brief checks if the current state is done processing
 */
void arkanix_check_state()
{
	switch (arkanix->state)
	{

	case MAIN_MENU:

		if (((menu_t*) arkanix->statep)->done)
		{
			switch (((menu_t*) arkanix->statep)->action)
			{

			case PLAY_SINGLEPLAYER:

				arkanix->mode = (((menu_t*) arkanix->statep)->optionsToggle) ? MODE_KEYBOARD : MODE_MOUSE;
				arkanix_change_state(SINGLEPLAYER);
				break;

			case PLAY_MULTIPLAYER_LOCAL:

				arkanix_change_state(MULTIPLAYER_LOCAL);
				break;

			case PLAY_MULTIPLAYER_SERIAL:

				arkanix_change_state(MULTIPLAYER_SERIAL);
				break;

			case QUIT:

				arkanix->done = 1;
				break;
			}
		}

		break;

	case SINGLEPLAYER:

		if (((level_t*) arkanix->statep)->done)
		{
			arkanix->lives = ((level_t*) arkanix->statep)->player->lives;
			arkanix->score = ((level_t*) arkanix->statep)->player->score;

			table_t* hs_table = (table_t*) score_get();

			if (((level_t*) arkanix->statep)->state == EXIT_MENU)
			{
				arkanix_change_state(MAIN_MENU);
				return;
			}
			if (arkanix->lives <= 0)
			{
				if (hs_table->cur_size < hs_table->max_size)
				{
					arkanix_change_state(SCORE_SCREEN);
				}
				else if (hs_table->table[hs_table->cur_size - 1].points < arkanix->score)
				{
					arkanix_change_state(SCORE_SCREEN);
				}
				else
				{
					arkanix_change_state(MAIN_MENU);
				}
				return;
			}
			if (arkanix->level <= 2)
			{
				arkanix->level++;
				arkanix_change_state(SINGLEPLAYER);
			}
			else
			{
				if (hs_table->cur_size < hs_table->max_size)
				{
					arkanix_change_state(SCORE_SCREEN);
				}
				else if (hs_table->table[hs_table->cur_size - 1].points < arkanix->score)
				{
					arkanix_change_state(SCORE_SCREEN);
				}
				else
				{
					arkanix_change_state(MAIN_MENU);
				}
				return;
			}
		}

		break;

	case MULTIPLAYER_LOCAL:
	case MULTIPLAYER_SERIAL:

		if (((ss_t*) arkanix->statep)->done)
		{
			if (((ss_t*) arkanix->statep)->state == EXIT_MENU)
			{
				arkanix_change_state(MAIN_MENU);
				return;
			}
			arkanix_change_state(MAIN_MENU);
		}

		break;

	case SCORE_SCREEN:

		if (hss->done)
		{
			score_write();
			arkanix_change_state(MAIN_MENU);
		}

		break;

	}

}

/**
 * @brief handles keyboard interrupts for each game mode
 * - MAIN_MENU: N/A
 * - SINGLEPLAYER: updates paddle movement IF player has set control mode to "Keboard"
 * - MULTIPLAYER_SERIAL: updates player 1 paddle movement
 * - MULTIPLAYER_LOCAL: updates player 2 paddle movemenet
 */
void arkanix_handle_keyboard()
{
	arkanix->kbd_scancode = kbd_handler();

	if (arkanix->state == SINGLEPLAYER)
	{
		((level_t*) arkanix->statep)->scancode = arkanix->kbd_scancode;

		if (arkanix->mode == MODE_KEYBOARD)
		{
			paddle_keyboard_update(((level_t*) arkanix->statep)->player->paddle,
					((level_t*) arkanix->statep)->player->bounds,
					arkanix->kbd_scancode);
		}

		((level_t*) arkanix->statep)->draw_active = 1;
	}
	else if (arkanix->state == MULTIPLAYER_SERIAL)
	{
		mouse_t* ss_paddle = ((ss_t*) arkanix->statep)->p1->paddle;
		rectangle_t* ss_bounds = ((ss_t*) arkanix->statep)->p1->bounds;

		if (((ss_t*) arkanix->statep)->state == PLAYING)
		{
			paddle_keyboard_update(ss_paddle, ss_bounds, arkanix->kbd_scancode);
			serial_send_paddle((unsigned char) (arkanix->kbd_scancode & 0xff));
			((ss_t*) arkanix->statep)->draw_active = 1;
		}
		else
		{
			keyboard_update_buttons(ss_paddle, arkanix->kbd_scancode);
			((ss_t*) arkanix->statep)->draw_active = 0;
		}
	}
	else if (arkanix->state == MULTIPLAYER_LOCAL)
	{
		mouse_t* p1_paddle = ((ss_t*) arkanix->statep)->p1->paddle;
		rectangle_t* p1_bounds = ((ss_t*) arkanix->statep)->p1->bounds;

		if (((ss_t*) arkanix->statep)->state == PLAYING)
		{
			paddle_keyboard_update(p1_paddle, p1_bounds, arkanix->kbd_scancode);
			((ss_t*) arkanix->statep)->draw_active = 1;
		}
		else
		{
			keyboard_update_buttons(p1_paddle, arkanix->kbd_scancode);
			((ss_t*) arkanix->statep)->draw_active = 0;
		}

	}
}

/**
 * @brief handles real-time clock interrupts (multiplayer mode only!)
 * - Alarm Interrupts (RTC_AF)
 * - Update Interrupts (RTC_UF)
 */
void arkanix_handle_rtc()
{
	unsigned long ev = rtc_check();

	if (arkanix->state == MULTIPLAYER_SERIAL || arkanix->state == MULTIPLAYER_LOCAL)
	{
		if (ev & RTC_AF)
		{
			((ss_t*) arkanix->statep)->done = 1;
		}
		if (ev & RTC_UF)
		{
			if (((ss_t*) arkanix->statep)->state == PLAYING && ((ss_t*) arkanix->statep)->start)
			{
				((ss_t*) arkanix->statep)->time--;
			}
		}
	}
}

/**
 * @brief handles mouse interrupts for each game mode
 * - SINGLEPLAYER: updates paddle movement IF player has set control mode to "Mouse"
 * - MULTIPLAYER_SERIAL: not used
 * - MULTIPLAYER_LOCAL: updates player 1 paddle movement
 */
void arkanix_handle_mouse()
{
	if (arkanix->state == MAIN_MENU)
	{
		mouse_t* menu_cursor = ((menu_t*) arkanix->statep)->mouse;
		rectangle_t* menu_bounds = ((menu_t*) arkanix->statep)->bounds;
		mouse_handler(menu_cursor, menu_bounds);
	}

	else if (arkanix->state == SINGLEPLAYER && arkanix->mode == MODE_MOUSE)
	{
		mouse_t* sp_paddle = ((level_t*) arkanix->statep)->player->paddle;
		rectangle_t* sp_bounds = ((level_t*) arkanix->statep)->player->bounds;
		mouse_handler(sp_paddle, sp_bounds);

		if (sp_paddle->updated)
		{
			((level_t*) arkanix->statep)->draw_active = 1;
		}
		else
		{
			((level_t*) arkanix->statep)->draw_active = 0;
		}
	}

	else if (arkanix->state == MULTIPLAYER_LOCAL)
	{
		mouse_t* ss_paddle = ((ss_t*) arkanix->statep)->p2->paddle;
		rectangle_t* ss_bounds = ((ss_t*) arkanix->statep)->p2->bounds;
		mouse_handler(ss_paddle, ss_bounds);

		if (((ss_t*) arkanix->statep)->state == PLAYING && ss_paddle->updated)
		{
			((ss_t*) arkanix->statep)->draw_active = 1;
		}
		else
		{
			((ss_t*) arkanix->statep)->draw_active = 0;
		}
	}
}

/**
 * @brief handles timer interrupts for each game mode
 * - MAIN_MENU: makes timer tick :)
 * - SINGLEPLAYER: makes timer tick :)
 * - MULTIPLAYER_SERIAL: makes timer tick, sends periodic data packets through serial port
 * - MULTIPLAYER_LOCAL: makes timer tick :)
 */
void arkanix_handle_timer()
{
	timer_handler();
}

void serial_update_paddle()
{
	mouse_t* p2_paddle = ((ss_t*) arkanix->statep)->p2->paddle;
	rectangle_t* p2_bounds = ((ss_t*) arkanix->statep)->p2->bounds;

	if (((ss_t*) arkanix->statep)->state == PLAYING)
	{
		paddle_keyboard_update(p2_paddle, p2_bounds, arkanix->ser_scancode);
		((ss_t*) arkanix->statep)->draw_active = 1;
	}
	else if (((ss_t*) arkanix->statep)->state == WAITING_PLAYER)
	{
		if (arkanix->ser_scancode == 0x55)
		{
			((ss_t*) arkanix->statep)->p2_ready = 1;
			serial_write_char(0x55);
		}
	}
	else
	{
		keyboard_update_buttons(p2_paddle, arkanix->ser_scancode);
		((ss_t*) arkanix->statep)->draw_active = 0;
	}
}

/**
 * @brief handles serial port interrupts (COM1)
 * - MULTIPLAYER_SERIAL: checks if serial port has recieved data
 * interrupts and updates player 2 paddle movement accordingly..
 * @warning serial multiplayer only
 */
void arkanix_handle_serial()
{
	unsigned long d = 0;
	unsigned long r = 0;

	if (arkanix->state == MULTIPLAYER_SERIAL)
	{
		sys_inb(COM1_PORT + II_REG, &r);

		if (r & SERIAL_NO_INT)
		{
			printf("no pending interrupts...\n");
		}
		else
		{
			if ((r & SERIAL_LSR_INT) == SERIAL_LSR_INT)
			{
				sys_inb(COM1_PORT + LS_REG, &d);
				printf("recieved error: 0x%x\n", (unsigned char) d & 0xff);
			}

			if ((r & SERIAL_RX_INT) == SERIAL_RX_INT)
			{
				d = serial_read_char();

				if (d != 0)
				{
					arkanix->ser_scancode = serial_recieve_paddle(d);
					printf("recieved packet: 0x%x\n", (unsigned char) d & 0xff);
					serial_update_paddle();
				}
			}
		}
	}
}

/**
 * @brief main update function
 * - receives device interrupt notifications
 * - calls the corresponding interrupt handler for each device
 * - updates game state accordingly
 */
void arkanix_update()
{
	int ipc_status;
	message msg;

	int event = 0;

	if (driver_receive(ANY, &msg, &ipc_status) != OK)
	{
		return;
	}

	if (is_ipc_notify(ipc_status))
	{
		if (_ENDPOINT_P(msg.m_source) == HARDWARE)
		{
			if (msg.NOTIFY_ARG & arkanix->mouse_hook)
			{
				event |= arkanix->mouse_hook;
			}
			if (msg.NOTIFY_ARG & arkanix->kbd_hook)
			{
				event |= arkanix->kbd_hook;
			}
			if (msg.NOTIFY_ARG & arkanix->serial_hook)
			{
				event |= arkanix->serial_hook;
			}
			if (msg.NOTIFY_ARG & arkanix->timer_hook)
			{
				event |= arkanix->timer_hook;
			}
			if (msg.NOTIFY_ARG & arkanix->rtc_hook)
			{
				event |= arkanix->rtc_hook;
			}
		}
	}

	if (event & arkanix->mouse_hook)
	{
		arkanix_handle_mouse();
	}
	if (event & arkanix->kbd_hook)
	{
		arkanix_handle_keyboard();
	}
	if (event & arkanix->timer_hook)
	{
		arkanix_handle_timer();
	}
	if (event & arkanix->rtc_hook)
	{
		arkanix_handle_rtc();
	}
	if (event & arkanix->serial_hook)
	{
		arkanix_handle_serial();
	}

	if (timer_ticked())
	{
		switch (arkanix->state)
		{

		case MAIN_MENU:

			menu_draw((menu_t*) arkanix->statep);
			menu_update((menu_t*) arkanix->statep, arkanix->kbd_scancode);

			break;

		case SINGLEPLAYER:

			if (arkanix->mode == MODE_KEYBOARD) {
				paddle_keyboard_refresh(
						((level_t*) arkanix->statep)->player->paddle,
						((level_t*) arkanix->statep)->player->bounds);
			}

			level_update((level_t*) arkanix->statep);

			break;

		case MULTIPLAYER_SERIAL:

			if (((ss_t*) arkanix->statep)->state == PLAYING)
			{
				paddle_keyboard_refresh(((ss_t*) arkanix->statep)->p1->paddle,
						((ss_t*) arkanix->statep)->p1->bounds);
				paddle_keyboard_refresh(((ss_t*) arkanix->statep)->p2->paddle,
						((ss_t*) arkanix->statep)->p2->bounds);
			}

			ss_update((ss_t*) arkanix->statep);

			break;

		case MULTIPLAYER_LOCAL:

			if (((ss_t*) arkanix->statep)->state == PLAYING)
			{
				paddle_keyboard_refresh(((ss_t*) arkanix->statep)->p1->paddle,
						((ss_t*) arkanix->statep)->p1->bounds);
			}

			ss_update((ss_t*) arkanix->statep);

			break;

		case SCORE_SCREEN:

			hss_update();
			hss_draw();

			break;

		}

		arkanix->kbd_scancode = 0;
		arkanix->ser_scancode = 0;

		arkanix_check_state();
		timer_reset();
	}
}

/**
 * @brief terminates game
 */
void arkanix_stop()
{
	kbd_unsubscribe();

	rtc_disable();
	rtc_unsubscribe();

	serial_enable();
	serial_unsubscribe();

	timer_unsubscribe();
	timer_destroy();

	free(arkanix);
}

/**
 * @brief entry function, main game cycle
 */
int main()
{
	// initializes pseudo-random number generation
	srand(time(NULL));

	// initializes system services
	sef_startup();
	sys_enable_iop(SELF);

	// initializes and reads high scores
	score_create();
	score_read();

	// initializes video mode
	video_init();

	// initializes game state
	arkanix_start();
	arkanix_change_state(MAIN_MENU);

	// main game loop
	while (!arkanix->done)
	{
		arkanix_update();
	}

	// saves updated high scores
	score_destroy();

	// frees all the allocated memory
	// unsubscribes device interrupts
	arkanix_stop();

	// returns to text mode
	video_exit();

	return 0;
}
