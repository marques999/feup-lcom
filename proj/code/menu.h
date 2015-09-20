#ifndef __MENU_H
#define __MENU_H

#include "bitmap.h"
#include "input.h"
#include "video.h"

typedef enum {
	DEFAULT, PLAY_SINGLEPLAYER, PLAY_MULTIPLAYER, QUIT, PLAY_MULTIPLAYER_LOCAL, PLAY_MULTIPLAYER_SERIAL
} menu_s;

typedef struct
{
	short* background;
	short* menu_buffer;
	short* active_buffer;

	bitmap_t* singleplayer_normal;
	bitmap_t* singleplayer_hover;

	bitmap_t* multiplayer_normal;
	bitmap_t* multiplayer_hover;

	bitmap_t* local_normal;
	bitmap_t* local_hover;

	bitmap_t* serial_normal;
	bitmap_t* serial_hover;

	bitmap_t* exit_normal;
	bitmap_t* exit_hover;

	bitmap_t* back_normal;
	bitmap_t* back_hover;

	bitmap_t* mouse_normal;
	bitmap_t* mouse_hover;

	bitmap_t* keyboard_normal;
	bitmap_t* keyboard_hover;

	menu_s action;

	mouse_t* mouse;

	rectangle_t* bounds;
	rectangle_t* singleplayerButton;
	rectangle_t* multiplayerButton;
	rectangle_t* exitButton;
	rectangle_t* optionsButton;

	int singleplayerHover;
	int multiplayerHover;
	int localHover;
	int serialHover;
	int backHover;
	int exitHover;
	int optionsHover;

	int optionsToggle;

	int draw;
	int draw_mouse;

	int done;

} menu_t;

menu_t* menu_create();
void menu_delete(menu_t* menu);
void menu_reset(menu_t* menu);
void menu_draw(menu_t* menu);
void menu_update(menu_t* menu, unsigned long scancode);

#endif /* __MENU_H */
