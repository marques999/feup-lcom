#ifndef __MOUSE_H
#define __MOUSE_H

#include "i8042.h"
#include "timer.h"

/** @defgroup mouse mouse
 * @{
 *
 * Functions for testing the kbd code
 */

#define MOUSE_IRQ 12

#define MOUSE_LB BIT(0)
#define MOUSE_RB BIT(1)
#define MOUSE_MB BIT(2)
#define MOUSE_UNUSED BIT(3) // always set
#define MOUSE_X_SIGN BIT(4)
#define MOUSE_Y_SIGN BIT(5)
#define MOUSE_X_OVF BIT(6)
#define MOUSE_Y_OVF BIT(7)

typedef struct {
	unsigned char packet[3];
	unsigned short count;
	short accu;
	unsigned short tolerance;
	int synced;
} packet_t;

typedef enum {
	INIT, DRAW, COMP
} state_t;

typedef enum {
	LDOWN, LUP, MOVE
} ev_type_t;

void mouse_check_line(ev_type_t* evt, packet_t* data);
void mouse_get_conf(unsigned char* packet);
short mouse_get_pos(unsigned char value, unsigned char sign);
int mouse_int_handler(packet_t *data);

int mouse_read(unsigned char *data);
unsigned char mouse_sync();
int mouse_write(unsigned char data);
int mouse_write_cmd(unsigned char cmd, unsigned char args);

int mouse_subscribe_int();
int mouse_unsubscribe_int();

#endif
