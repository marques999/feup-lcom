#include "level.h"
#include "ss.h"

// GGGGGGGGG
//  BBBBBBBB
//   GGGGGGG
//    YYYYYY
//     OOOOO
//      RRRR

void generate_level1(level_t* level) {
	int x, y;
	int i, j;

	int width = level->player->block_width;
	int height = level->player->block_height;

	level->player->num_blocks = 0;
	y = level->player->block_bounds->yi;

	for (j = 0; j < level->player->num_columns; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, GREY_BLOCK);
		level->player->num_blocks++;
	}

	y += level->player->block_height + 6;

	for (j = 1; j < level->player->num_columns; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, BLUE_BLOCK);
		level->player->num_blocks++;
	}

	y += level->player->block_height + 6;

	for (j = 2; j < level->player->num_columns; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, GREEN_BLOCK);
		level->player->num_blocks++;
	}

	y += level->player->block_height + 6;

	for (j = 3; j < level->player->num_columns; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, YELLOW_BLOCK);
		level->player->num_blocks++;
	}

	y += level->player->block_height + 6;

	for (j = 4; j < level->player->num_columns; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, PURPLE_BLOCK);
		level->player->num_blocks++;
	}

	y += level->player->block_height + 6;

	for (j = 5; j < level->player->num_columns; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, RED_BLOCK);
		level->player->num_blocks++;
	}
}

//    XBX
//   XGGGX
//  XRRRRRX
// XOOOOOOOX
//  XRRRRRX
//   XGGGX
//	  XBX

void generate_level2(level_t* level) {

	int x;
	int y;
	int i;
	int j;

	int width = level->player->block_width;
	int height = level->player->block_height;

	level->player->num_blocks = 0;
	y = level->player->block_bounds->yi;

	/**
	 * 000XBX000
	 */

	x = level->player->block_bounds->xi + 3 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 4 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, BLUE_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 5 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	y += level->player->block_height + 6;

	/**
	 * 00XGGGX00
	 */

	x = level->player->block_bounds->xi + 2 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	for (j = 3; j < 6; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, GREEN_BLOCK);
		level->player->num_blocks++;
	}

	x = level->player->block_bounds->xi + 6 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	y += level->player->block_height + 6;

	/**
	 * 0XRRRRRX0
	 */

	x = level->player->block_bounds->xi + 1 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	for (j = 2; j < 7; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, RED_BLOCK);
		level->player->num_blocks++;
	}

	x = level->player->block_bounds->xi + 7 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	y += level->player->block_height + 6;

	/**
	 * XOOOOOOOX
	 */

	x = level->player->block_bounds->xi + 0 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	for (j = 1; j < 8; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, PURPLE_BLOCK);
		level->player->num_blocks++;
	}

	x = level->player->block_bounds->xi + 8 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	y += level->player->block_height + 6;

	/**
	 * 0XRRRRRX0
	 */

	x = level->player->block_bounds->xi + 1 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	for (j = 2; j < 7; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, RED_BLOCK);
		level->player->num_blocks++;
	}

	x = level->player->block_bounds->xi + 7 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;


	y += level->player->block_height + 6;

	/**
	 * 00XGGGX00
	 */

	x = level->player->block_bounds->xi + 2 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	for (j = 3; j < 6; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, GREEN_BLOCK);
		level->player->num_blocks++;
	}

	x = level->player->block_bounds->xi + 6 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	y += level->player->block_height + 6;

	/**
	 * 000XBX000
	 */

	x = level->player->block_bounds->xi + 3 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 4 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, BLUE_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 5 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, GREY_BLOCK);
	level->player->num_blocks++;
}

//    XBX
//   XGGGX
//  XRRRRRX
// XOOOOOOOX
//  XRRRRRX
//   XGGGX
//	  XBX

void generate_level3(level_t* level) {

	level->player->num_rows = 6;
	level->player->num_columns = 19;
	level->player->block_height = 26;
	level->player->block_width = 24;
	level->player->block_bounds->xi -= 8;

	int x;
	int y;
	int i;
	int j;

	int width = level->player->block_width;
	int height = level->player->block_height;

	level->player->num_blocks = 0;
	y = level->player->block_bounds->yi;

	for (j = 0; j < 4; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, FEUP_BLOCK);
		level->player->num_blocks++;
	}

	for (j = 5; j < 9; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, FEUP_BLOCK);
		level->player->num_blocks++;
	}

	x = level->player->block_bounds->xi + 10 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 13 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	for (j = 15; j < 19; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, FEUP_BLOCK);
		level->player->num_blocks++;
	}

	y += level->player->block_height + 6;

	x = level->player->block_bounds->xi + 0 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 5 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 10 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 13 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 15 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 18 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	y += level->player->block_height + 6;

	for (j = 0; j < 3; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, FEUP_BLOCK);
		level->player->num_blocks++;
	}

	for (j = 5; j < 9; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, FEUP_BLOCK);
		level->player->num_blocks++;
	}

	x = level->player->block_bounds->xi + 10 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 13 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	for (j = 15; j < 19; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, FEUP_BLOCK);
		level->player->num_blocks++;
	}

	y += level->player->block_height + 6;

	x = level->player->block_bounds->xi + 0 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 5 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 10 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 13 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	x = level->player->block_bounds->xi + 15 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	y += level->player->block_height + 6;

	x = level->player->block_bounds->xi + 0 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

	for (j = 5; j < 9; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, FEUP_BLOCK);
		level->player->num_blocks++;
	}

	for (j = 10; j < 14; i++, j++) {
		x = level->player->block_bounds->xi + j * (level->player->block_width + 4);
		level->player->blocks[i] = block_create(x, y, width, height, FEUP_BLOCK);
		level->player->num_blocks++;
	}

	x = level->player->block_bounds->xi + 15 * (level->player->block_width + 4);
	level->player->blocks[i++] = block_create(x, y, width, height, FEUP_BLOCK);
	level->player->num_blocks++;

}

// GGGGGGGGG
// BBBBBBBBB
// GGGGGGGGG
// YYYYYYYYY
// OOOOOOOOO
// RRRRRRRRR

void ss_generate(ss_t* splitscreen) {

	int x1;
	int x2;
	int y1;
	int y2;
	int i;
	int j;

	int width = splitscreen->p1->block_width;
	int height = splitscreen->p1->block_height;

	splitscreen->p1->num_blocks = 0;
	splitscreen->p2->num_blocks = 0;

	y1 = splitscreen->p1->block_bounds->yi;
	y2 = splitscreen->p2->block_bounds->yi;

	for (j = 0; j < splitscreen->p1->num_columns; i++, j++) {
		x1 = splitscreen->p1->block_bounds->xi + j * (splitscreen->p1->block_width + 4);
		x2 = splitscreen->p2->block_bounds->xi + j * (splitscreen->p2->block_width + 4);
		splitscreen->p1->blocks[i] = block_create(x1, y1, width, height, GREY_BLOCK);
		splitscreen->p2->blocks[i] = block_create(x2, y2, width, height, GREY_BLOCK);
		splitscreen->p1->num_blocks++;
		splitscreen->p2->num_blocks++;
	}

	y1 += splitscreen->p1->block_height + 6;
	y2 += splitscreen->p2->block_height + 6;

	for (j = 0; j < splitscreen->p1->num_columns; i++, j++) {
		x1 = splitscreen->p1->block_bounds->xi + j * (splitscreen->p1->block_width + 4);
		x2 = splitscreen->p2->block_bounds->xi + j * (splitscreen->p2->block_width + 4);
		splitscreen->p1->blocks[i] = block_create(x1, y1, width, height, BLUE_BLOCK);
		splitscreen->p2->blocks[i] = block_create(x2, y2, width, height, BLUE_BLOCK);
		splitscreen->p1->num_blocks++;
		splitscreen->p2->num_blocks++;
	}

	y1 += splitscreen->p1->block_height + 6;
	y2 += splitscreen->p2->block_height + 6;

	for (j = 0; j < splitscreen->p1->num_columns; i++, j++) {
		x1 = splitscreen->p1->block_bounds->xi + j * (splitscreen->p1->block_width + 4);
		x2 = splitscreen->p2->block_bounds->xi + j * (splitscreen->p2->block_width + 4);
		splitscreen->p1->blocks[i] = block_create(x1, y1, width, height, GREEN_BLOCK);
		splitscreen->p2->blocks[i] = block_create(x2, y2, width, height, GREEN_BLOCK);
		splitscreen->p1->num_blocks++;
		splitscreen->p2->num_blocks++;
	}

	y1 += splitscreen->p1->block_height + 6;
	y2 += splitscreen->p2->block_height + 6;

	for (j = 0; j < splitscreen->p1->num_columns; i++, j++) {
		x1 = splitscreen->p1->block_bounds->xi + j * (splitscreen->p1->block_width + 4);
		x2 = splitscreen->p2->block_bounds->xi + j * (splitscreen->p2->block_width + 4);
		splitscreen->p1->blocks[i] = block_create(x1, y1, width, height, YELLOW_BLOCK);
		splitscreen->p2->blocks[i] = block_create(x2, y2, width, height, YELLOW_BLOCK);
		splitscreen->p1->num_blocks++;
		splitscreen->p2->num_blocks++;
	}

	y1 += splitscreen->p1->block_height + 6;
	y2 += splitscreen->p2->block_height + 6;

	for (j = 0; j < splitscreen->p1->num_columns; i++, j++) {
		x1 = splitscreen->p1->block_bounds->xi + j * (splitscreen->p1->block_width + 4);
		x2 = splitscreen->p2->block_bounds->xi + j * (splitscreen->p2->block_width + 4);
		splitscreen->p1->blocks[i] = block_create(x1, y1, width, height, PURPLE_BLOCK);
		splitscreen->p2->blocks[i] = block_create(x2, y2, width, height, PURPLE_BLOCK);
		splitscreen->p1->num_blocks++;
		splitscreen->p2->num_blocks++;
	}

	y1 += splitscreen->p1->block_height + 6;
	y2 += splitscreen->p2->block_height + 6;

	for (j = 0; j < splitscreen->p1->num_columns; i++, j++) {
		x1 = splitscreen->p1->block_bounds->xi + j * (splitscreen->p1->block_width + 4);
		x2 = splitscreen->p2->block_bounds->xi + j * (splitscreen->p2->block_width + 4);
		splitscreen->p1->blocks[i] = block_create(x1, y1, width, height, RED_BLOCK);
		splitscreen->p2->blocks[i] = block_create(x2, y2, width, height, RED_BLOCK);
		splitscreen->p1->num_blocks++;
		splitscreen->p2->num_blocks++;
	}
}
