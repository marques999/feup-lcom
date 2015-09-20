#ifndef __SCORE_H
#define __SCORE_H

#include "rtc.h"

/**
 * @brief highscore entry
 */
typedef struct
{
	char* name; ///> player name
	rtc_time_t date; ///> highscore time and date
	unsigned points; ///> player score
} score_t;

/**
 * @brief highscores table
 */
typedef struct
{
	score_t* table; ///> highscores table entries

	unsigned cur_size; ///> table current size
	unsigned max_size; ///> table maximum size (10)
	unsigned length; ///> player name maximum size (4)

} table_t;

/**
 * @brief creates a new instance of table_t class
 */
void score_create();

/**
 * @brief reads highscore entries from a file
 */
void score_read();

/**
 * @brief writes highscores to a file
 */
void score_write();

/**
 * @brief inserts a new highscore into the table, and then sorts it
 * @param player_name player name
 * @param player_score player score
 */
void score_add(char* player_name, int player_score);

#endif /* __SCORE_H */
