#include "mouse.h"

int mouse_hook = 2;

#define SIGN_EXTEND 0xff00
#define ZERO_EXTEND 0x00ff

short mouse_get_pos(unsigned char value, unsigned char sign) {
	return sign ? value | SIGN_EXTEND : value & ZERO_EXTEND;
}

int mouse_read(unsigned char *data) {
	unsigned char status = 0;
	sys_inb(STAT_REG, (unsigned long *) &status);
	if (status & (OBF | AUX)) {
		if (sys_inb(OUT_BUF, (unsigned long *) data) != OK) {
			return ERROR;
		}
		return OK;
	}
}

unsigned char mouse_sync() {
	unsigned char data = 0;
	while (1) {
		tickdelay(micros_to_ticks(DELAY));
		if (mouse_read(&data) != OK) {
			return ERROR;
		}
		if (data == 0xff) {
			continue;
		}
		if (data & MOUSE_UNUSED) {
			return data;
		}
	}
}
int mouse_write(unsigned char data) {
	unsigned char status = 0;
	sys_inb(STAT_REG, (unsigned long *) &status);
	if ((status & IBF) == 0) {
		if (sys_outb(CMD_REG, MOUSE_CMD) != OK) {
			return ERROR;
		}
		if (sys_outb(IN_BUF, data) != OK) {
			return ERROR;
		}
		tickdelay(micros_to_ticks(DELAY));
		mouse_read(&status);
		return status;
	}
}

int mouse_subscribe_int() {
	mouse_write(STREAM_OFF);
	int bit_order = mouse_hook;
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook) != 0) {
		printf("mouse_subscribe_int()::kernel call didn't return 0\n");
		return ERROR;
	}
	if (sys_irqenable(&mouse_hook) != 0) {
		printf("mouse_subscribe_int()::kernel call didn't return 0\n");
		return ERROR;
	}
	mouse_write(STREAM_ENABLE);
	mouse_write(STREAM_ON);
	return BIT(bit_order);
}

int mouse_unsubscribe_int() {
	if (sys_irqdisable(&mouse_hook) != 0) {
		printf("mouse_subscribe_int()::kernel call didn't return 0\n");
		return ERROR;
	}
	if (sys_irqrmpolicy(&mouse_hook) != 0) {
		printf("mouse_subscribe_int()::kernel call didn't return 0\n");
		return ERROR;
	}
	mouse_write(STREAM_OFF);
	return 0;
}

void mouse_check_line(ev_type_t* evt, packet_t* data) {
	static state_t state = INIT;

	short posX = mouse_get_pos(data->packet[1], data->packet[0] & MOUSE_X_SIGN ? 1 : 0);
	short posY = mouse_get_pos(data->packet[2], data->packet[0] & MOUSE_Y_SIGN ? 1 : 0);

	if (data->packet[0] & MOUSE_LB) {
		if (state == DRAW && posX != 0) {
			*evt = MOVE;
		}
		else {
			*evt = LDOWN;
		}
	} else if ((data->packet[0] & MOUSE_LB) == 0) {
		*evt = LUP;
	}

	switch (state) {
	case INIT:
		if (*evt == LDOWN) {
			state = DRAW;
			data->accu = 0;
		}
		break;
	case DRAW:
		if (*evt == MOVE) {
			if (abs(posY) <= data->tolerance) {
				data->accu = data->accu + posX;
			} else {
				data->accu = 0;
			}
		} else if (*evt == LUP) {
			state = INIT;
		}
		break;
	case COMP:
		break;
	}
}

void mouse_get_conf(unsigned char* packet) {
	// B1=0x8	B2=0x12	B3=0x14
	printf("B1=0x%X   B2=0x%X   B3=0x%X   ", packet[0], packet[1], packet[2]);
	// LB=0		MB=0	RB=0	XOV=0	YOV=0
	printf("LB=%d   ", packet[0] & MOUSE_LB ? 1 : 0);
	printf("MB=%d   ", packet[0] & MOUSE_MB ? 1 : 0);
	printf("RB=%d   ", packet[0] & MOUSE_RB ? 1 : 0);
	printf("XOV=%d   ", packet[0] & MOUSE_X_OVF ? 1 : 0);
	printf("YOV=%d   ", packet[0] & MOUSE_Y_OVF ? 1 : 0);
	// X=18		Y=20
	unsigned char Xsign = packet[0] & MOUSE_X_SIGN ? 1 : 0;
	unsigned char Ysign = packet[0] & MOUSE_Y_SIGN ? 1 : 0;
	printf("X=%d   ", mouse_get_pos(packet[1], Xsign));
	printf("Y=%d\n", mouse_get_pos(packet[2], Ysign));
}

int mouse_int_handler(packet_t *data) {
	unsigned char result = 0;
	while (1) {
		if (!data->synced) {
			data->packet[0] = mouse_sync();
			data->count++;
			data->synced = 1;
			break;
		} else {
			if (mouse_read(&result) != 0) {
				printf("mouse_int_handler()::mouse_read didn't return 0\n");
				return ERROR;
			}
			if (data->count == 0 && (result & MOUSE_UNUSED) == 0) {
				printf("mouse_int_handler()::lost sync\n");
				data->synced = 0;
				continue;
			}
			data->packet[data->count++] = result;
			break;
		}
	}
	return OK;
}
