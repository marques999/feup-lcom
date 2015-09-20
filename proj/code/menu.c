#include "menu.h"

menu_t* menu_create() {

	menu_t* new_menu = (menu_t*) malloc(sizeof(menu_t));

	const char* background_filename = "/home/lcom/arkanix/res/menu-0x117.bmp";
	new_menu->background = bitmap_copy(background_filename);

	new_menu->menu_buffer = (short*) buffer_create();
	new_menu->active_buffer = (short*) buffer_create();

	const char* cursor_filename = "/home/lcom/arkanix/res/pointer-0x117.bmp";
	new_menu->mouse = mouse_create(cursor_filename, CURSOR, 512, 384);

	new_menu->bounds = (rectangle_t*) create_rectangle(0, 0, 1024, 768);

	new_menu->singleplayer_normal = bitmap_read(
			"/home/lcom/arkanix/res/singleplayer_normal-0x117.bmp");
	new_menu->singleplayer_hover = bitmap_read(
			"/home/lcom/arkanix/res/singleplayer_hover-0x117.bmp");

	new_menu->multiplayer_normal = bitmap_read(
			"/home/lcom/arkanix/res/multiplayer_normal-0x117.bmp");
	new_menu->multiplayer_hover = bitmap_read(
			"/home/lcom/arkanix/res/multiplayer_hover-0x117.bmp");

	new_menu->local_normal = bitmap_read(
			"/home/lcom/arkanix/res/local_normal-0x117.bmp");
	new_menu->local_hover = bitmap_read(
			"/home/lcom/arkanix/res/local_hover-0x117.bmp");

	new_menu->serial_normal = bitmap_read(
			"/home/lcom/arkanix/res/serial_normal-0x117.bmp");
	new_menu->serial_hover = bitmap_read(
			"/home/lcom/arkanix/res/serial_hover-0x117.bmp");

	new_menu->exit_normal = bitmap_read(
			"/home/lcom/arkanix/res/exit_normal-0x117.bmp");
	new_menu->exit_hover = bitmap_read(
			"/home/lcom/arkanix/res/exit_hover-0x117.bmp");

	new_menu->back_normal = bitmap_read(
			"/home/lcom/arkanix/res/back_normal-0x117.bmp");
	new_menu->back_hover = bitmap_read(
			"/home/lcom/arkanix/res/back_hover-0x117.bmp");

	new_menu->mouse_normal = bitmap_read(
			"/home/lcom/arkanix/res/mouse_normal-0x117.bmp");
	new_menu->mouse_hover = bitmap_read(
			"/home/lcom/arkanix/res/mouse_hover-0x117.bmp");

	new_menu->keyboard_normal = bitmap_read(
			"/home/lcom/arkanix/res/keyboard_normal-0x117.bmp");
	new_menu->keyboard_hover = bitmap_read(
			"/home/lcom/arkanix/res/keyboard_hover-0x117.bmp");

	const int buttonWidth = 275;
	const int buttonHeight = 275;
	const int buttonSingleplayerY = 334;
	const int buttonSingleplayerX = 255;
	const int buttonMultiplayerY = 334;
	const int buttonMultiplayerX = 530;
	const int buttonOptionsX = 321;
	const int buttonOptionsY = 638;
	const int buttonExitX = 578;
	const int buttonExitY = 638;
	const int buttonSmallWidth = 168;
	const int buttonSmallHeight = 50;

	new_menu->singleplayerButton = (rectangle_t*) create_rectangle(
			buttonSingleplayerX, buttonSingleplayerY,
			buttonSingleplayerX + buttonWidth,
			buttonSingleplayerY + buttonHeight);
	new_menu->multiplayerButton = (rectangle_t*) create_rectangle(
			buttonMultiplayerX, buttonMultiplayerY,
			buttonMultiplayerX + buttonWidth,
			buttonMultiplayerY + buttonHeight);
	new_menu->exitButton = (rectangle_t*) create_rectangle(buttonExitX,
			buttonExitY, buttonExitX + buttonSmallWidth,
			buttonExitY + buttonSmallHeight);
	new_menu->optionsButton = (rectangle_t*) create_rectangle(buttonOptionsX,
			buttonOptionsY, buttonOptionsX + buttonSmallWidth,
			buttonOptionsY + buttonSmallHeight);

	menu_reset(new_menu);

	new_menu->action = DEFAULT;
	new_menu->draw = 1;
	new_menu->draw_mouse = 1;
	new_menu->optionsToggle = 0;

	return new_menu;
}

void menu_delete(menu_t* menu) {

	// check if menu was already destroyed
	if (menu == NULL) {
		return;
	}

	// release memory
	buffer_destroy(menu->background);
	buffer_destroy(menu->menu_buffer);
	buffer_destroy(menu->active_buffer);

	// delete all bitmaps from memory
	bitmap_destroy(menu->singleplayer_normal);
	bitmap_destroy(menu->singleplayer_hover);

	bitmap_destroy(menu->multiplayer_normal);
	bitmap_destroy(menu->multiplayer_hover);

	bitmap_destroy(menu->local_normal);
	bitmap_destroy(menu->local_hover);

	bitmap_destroy(menu->serial_normal);
	bitmap_destroy(menu->serial_hover);

	bitmap_destroy(menu->exit_normal);
	bitmap_destroy(menu->exit_hover);

	bitmap_destroy(menu->back_normal);
	bitmap_destroy(menu->back_hover);

	bitmap_destroy(menu->keyboard_normal);
	bitmap_destroy(menu->keyboard_hover);

	bitmap_destroy(menu->mouse_normal);
	bitmap_destroy(menu->mouse_hover);

	// destroy mouse instance
	mouse_destroy(menu->mouse);

	// delete rectangles
	free(menu->bounds);

	free(menu->singleplayerButton);
	free(menu->multiplayerButton);
	free(menu->optionsButton);
	free(menu->exitButton);

	// deletes the menu pointer
	free(menu);
	menu = NULL;
}

void menu_reset(menu_t* menu) {

	menu->singleplayerHover = 0;
	menu->multiplayerHover = 0;
	menu->serialHover = 0;
	menu->localHover = 0;
	menu->backHover = 0;
	menu->optionsHover = 0;
	menu->exitHover = 0;
}

void menu_update(menu_t* menu, unsigned long scancode) {

	if (scancode == KEY_ESC) {
		menu->action = QUIT;
		menu->done = 1;
	}

	if (menu->action == DEFAULT) {

		if (menu->mouse->updated) {

			menu->singleplayerHover = mouse_inside(menu->mouse, menu->singleplayerButton) ? 1 : 0;
			menu->multiplayerHover = mouse_inside(menu->mouse, menu->multiplayerButton) ? 1 : 0;
			menu->optionsHover = mouse_inside(menu->mouse, menu->optionsButton) ? 1 : 0;
			menu->exitHover = mouse_inside(menu->mouse, menu->exitButton) ? 1 : 0;
			menu->draw = 1;
			menu->draw_mouse = 1;
		}

		if (menu->mouse->leftButtonReleased) {

			if (menu->singleplayerHover) {
				menu->action = PLAY_SINGLEPLAYER;
				menu->done = 1;
			} else if (menu->multiplayerHover) {
				menu->action = PLAY_MULTIPLAYER;
				menu->done = 0;
			} else if (menu->optionsHover) {
				menu->optionsToggle ^= 1;
				menu->done = 0;
			} else if (menu->exitHover) {
				menu->action = QUIT;
				menu->done = 1;
			}

			menu->draw = 1;
		}
	}

	else if (menu->action == PLAY_MULTIPLAYER) {

		if (menu->mouse->updated) {

			menu->localHover = mouse_inside(menu->mouse, menu->singleplayerButton) ? 1 : 0;
			menu->serialHover = mouse_inside(menu->mouse, menu->multiplayerButton) ? 1 : 0;
			menu->backHover = mouse_inside(menu->mouse, menu->optionsButton) ? 1 : 0;
			menu->draw = 1;
			menu->draw_mouse = 1;
		}

		if (menu->mouse->leftButtonReleased) {

			if (menu->localHover) {
				menu->action = PLAY_MULTIPLAYER_LOCAL;
				menu->done = 1;
			} else if (menu->serialHover) {
				menu->action = PLAY_MULTIPLAYER_SERIAL;
				menu->done = 1;
			} else if (menu->backHover) {
				menu_reset(menu);
				menu->action = DEFAULT;
			}

			menu->draw = 1;
		}
	}

	menu->mouse->leftButtonReleased = 0;
}

void menu_draw(menu_t* menu) {

	if (!menu->draw && !menu->draw_mouse) {
		return;
	}

	if (menu->draw) {

		buffer_copy(menu->background, menu->menu_buffer);

		if (menu->action == DEFAULT) {

			if (menu->singleplayerHover) {
				bitmap_draw(menu->singleplayer_hover, menu->menu_buffer,
						menu->singleplayerButton->xi,
						menu->singleplayerButton->yi);
			} else {
				bitmap_draw(menu->singleplayer_normal, menu->menu_buffer,
						menu->singleplayerButton->xi,
						menu->singleplayerButton->yi);
			}

			if (menu->multiplayerHover) {
				bitmap_draw(menu->multiplayer_hover, menu->menu_buffer,
						menu->multiplayerButton->xi,
						menu->multiplayerButton->yi);
			} else {
				bitmap_draw(menu->multiplayer_normal, menu->menu_buffer,
						menu->multiplayerButton->xi,
						menu->multiplayerButton->yi);
			}

			if (menu->exitHover) {
				bitmap_draw(menu->exit_hover, menu->menu_buffer,
						menu->exitButton->xi, menu->exitButton->yi);
			} else {
				bitmap_draw(menu->exit_normal, menu->menu_buffer,
						menu->exitButton->xi, menu->exitButton->yi);
			}

			if (menu->optionsToggle) {

				if (menu->optionsHover) {
					bitmap_draw(menu->keyboard_hover, menu->menu_buffer,
							menu->optionsButton->xi, menu->optionsButton->yi);
				} else {
					bitmap_draw(menu->keyboard_normal, menu->menu_buffer,
							menu->optionsButton->xi, menu->optionsButton->yi);
				}
			}

			else {

				if (menu->optionsHover) {
					bitmap_draw(menu->mouse_hover, menu->menu_buffer,
							menu->optionsButton->xi, menu->optionsButton->yi);
				} else {
					bitmap_draw(menu->mouse_normal, menu->menu_buffer,
							menu->optionsButton->xi, menu->optionsButton->yi);
				}
			}

		}

		else if (menu->action == PLAY_MULTIPLAYER) {

			if (menu->localHover) {
				bitmap_draw(menu->local_hover, menu->menu_buffer,
						menu->singleplayerButton->xi,
						menu->singleplayerButton->yi);
			} else {
				bitmap_draw(menu->local_normal, menu->menu_buffer,
						menu->singleplayerButton->xi,
						menu->singleplayerButton->yi);
			}

			if (menu->serialHover) {
				bitmap_draw(menu->serial_hover, menu->menu_buffer,
						menu->multiplayerButton->xi,
						menu->multiplayerButton->yi);
			} else {
				bitmap_draw(menu->serial_normal, menu->menu_buffer,
						menu->multiplayerButton->xi,
						menu->multiplayerButton->yi);
			}

			if (menu->backHover) {
				bitmap_draw(menu->back_hover, menu->menu_buffer,
						menu->optionsButton->xi, menu->optionsButton->yi);
			} else {
				bitmap_draw(menu->back_normal, menu->menu_buffer,
						menu->optionsButton->xi, menu->optionsButton->yi);
			}
		}

		buffer_copy(menu->menu_buffer, menu->active_buffer);
	}

	if (menu->draw_mouse) {

		buffer_copy(menu->menu_buffer, menu->active_buffer);
		mouse_draw(menu->mouse, menu->active_buffer);
	}

	menu->draw = 0;
	menu->draw_mouse = 0;

	buffer_render(menu->active_buffer);
}
