#ifndef __ARKANIX_H
#define __ARKANIX_H

#include "video.h"
#include "score.h"
#include "sprite.h"
#include "input.h"

/**
 * @brief game state enumerator
 */
typedef enum {
	INIT, ///< initial state
	MAIN_MENU, ///< main menu
	SINGLEPLAYER, ///< singleplayer game
	MULTIPLAYER_LOCAL, ///< local multiplayer game (two opponents, same computer)
	MULTIPLAYER_SERIAL,///< serial multiplayer game (two opponents, two computers)
	SCORE_SCREEN ///< name entry screen
} GameState;

/**
 * @brief singleplayer mode player control options enumerator
 */
typedef enum {
	MODE_KEYBOARD, ///< player controls paddle using keyboard
	MODE_MOUSE ///< player controls paddle using mouse
} ControlMode;

/**
 * @brief name entry screen
 */
typedef struct {

	short* buffer; ///< graphics buffer

	char* player_name; ///< current player name
	int position; ///< current cursor position

	bitmap_t* victory_bitmap; ///< "YOU'RE WINNER" image
	sprite_t* underscore; ///< cursor image
	font_t* level_font; ///< default font

	int draw; ///< if set, draws cursor
	int done; ///< if set, exits name entry screen

} hsscreen_t;

/**
 * @brief main game struct
 */
typedef struct {

	int kbd_hook;  ///< keyboard interrupt bitmask
	int mouse_hook; ///<  mouse interrupt bitmask
	int serial_hook; ///< serial port interrupt bitmask
	int timer_hook; ///< timer interrupt bitmask
	int rtc_hook; ///< real-time-clock interupt bitmask

	keycode_t kbd_scancode; ///< player keyboard scancode
	keycode_t ser_scancode; ///< keyboard scancode recieved from serial port

	ControlMode mode; ///< chosen paddle control mode
	GameState state; ///< current game state
	void* statep; ///< pointer to the current state object

	int level; ///< current singleplayer level
	int score; ///< current player score
	int lives; ///< current player lives
	int done; ///< exits game

} ArkanixState;

#endif /* __ARKANIX_H */
