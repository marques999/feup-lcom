#include "kbd.h"
#include "timer.h"

#define KBD_IRQ			1
#define DELAY 			20000

/**
 * KBC registers
 */
#define STAT_REG 0x64
#define OUT_BUF 0x60

/**
 * KBC status bits
 */
#define OBF BIT(0)
#define IBF BIT(1)
#define TO_ERR BIT(5)
#define PAR_ERR BIT(7)

static int kbd_hook = 2;

int kbc_read(unsigned char *data) {
	unsigned char status = 0;
	int i;
	for (i = 0; i < 16; i++) {
		sys_inb(STAT_REG, (unsigned long *) &status);
		if ((status & OBF) != 0) {
			if (sys_inb(OUT_BUF, (unsigned long *) data) == OK)
				return OK;
		}
		tickdelay(micros_to_ticks(DELAY));
	}
	return ERROR;
}

int kbd_subscribe_int() {
	int bit_order = kbd_hook;
	if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook)
			!= 0) {
		return ERROR;
	}
	if (sys_irqenable(&kbd_hook) != 0) {
		return ERROR;
	}
	return BIT(bit_order);
}

int kbd_unsubscribe_int() {
	if (sys_irqdisable(&kbd_hook) != 0) {
		return ERROR;
	}
	if (sys_irqrmpolicy(&kbd_hook) != 0) {
		return ERROR;
	}
	return 0;
}

int kbd_int_handler() {
	unsigned short scancode;
	unsigned char value = 0;
	if (kbc_read(&value) != OK) {
		return ERROR;
	}
	if (value == 0xe0) {
		scancode = value << 8;
		if (kbc_read(&value) != OK) {
			return ERROR;
		}
		scancode |= value;
	} else {
		scancode = value;
	}
	return scancode;
}

int kbd_wait_input() {
	int ipc_status;
	int irq_set = kbd_subscribe_int();
	message msg;
	unsigned char result = 0;
	if (driver_receive(ANY, &msg, &ipc_status) != OK) {
		return ERROR;
	}
	if (is_ipc_notify(ipc_status)) {
		if(_ENDPOINT_P(msg.m_source) == HARDWARE) {
			if (msg.NOTIFY_ARG & irq_set) {
				kbc_read(&result);
			}
		}
	}
	kbd_unsubscribe_int();
	return result;
}
