#include "kbd.h"
#include "timer.h"

int kbd_hook = 2;

extern int kbd_int_handler_asm();

int kbc_read(unsigned char *data) {
	unsigned char status = 0;
	int i;
	for (i = 0; i < 16; i++) {
		sys_inb(STAT_REG, (unsigned long *) &status);
		if ((status & OBF) != 0) {
			if (sys_inb(OUT_BUF, (unsigned long *) data) != OK) {
				return ERROR;
			}
			return OK;
		}
		tickdelay(micros_to_ticks(DELAY));
	}
	return ERROR;
}

int kbc_write(unsigned char data) {
	unsigned char status = 0;
	int i;
	for (i = 0; i < 16; i++) {
		sys_inb(STAT_REG, (unsigned long *) &status);
		if ((status & IBF) == 0) {
			sys_outb(IN_BUF, data);
			return OK;
		}
		tickdelay(micros_to_ticks(DELAY));
	}
	return ERROR;
}

int kbc_write_cmd(unsigned char cmd, unsigned char args) {
	unsigned char result = 0;
	while (1) {
		if (kbc_write(cmd) != OK) {
			return ERROR;
		}
		kbc_read(&result);
		if (result != RSP_ACK) {
			continue;
		}
		while (1) {
			if (kbc_write(args) != OK) {
				return ERROR;
			}
			kbc_read(&result);
			if (result == RSP_ERROR) {
				break;
			}
			if (result == RSP_ACK) {
				return OK;
			}
		}
	}
	return ERROR;
}

int kbd_toggle_led(unsigned short bit, unsigned short* state) {
	if (bit == 0 || bit == 1 || bit == 2) {
		*state ^= KBD_LED(bit);
		if (kbc_write_cmd(CMD_LED, *state) != OK) {
			return ERROR;
		}
		return OK;
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

int kbd_int_handler_C() {
	unsigned short scancode;
	unsigned char value = 0;
	timer_count = 0;
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
	if (is_break_code(scancode)) {
		printf("\tBreakcode: 0x%X\n", scancode);
	} else {
		printf("\tMakecode: 0x%X\n", scancode);
	}
	return scancode;
}

int kbd_test_leds(unsigned short n, unsigned short *toggle) {
	unsigned short led_status = 0;
	kbd_subscribe_int();
	unsigned int i = 0;
	for (i = 0; i < n; i++) {
		if (kbd_toggle_led(toggle[i], &led_status) != 0) {
			printf("kbd_test_leds()::led number %d doesn't exist\n", toggle[i]);
			continue;
		}
		printf("kbd_test_leds()::0x0%X ", led_status);
		printf("(caps lock = %s, scroll lock = %s, num lock = %s)\n",
				led_status & CAPS_LOCK ? "o" : " ",
				led_status & SCROLL_LOCK ? "o" : " ",
				led_status & NUM_LOCK ? "o" : " ");
		timer_wait(12 /* 12/60 = 200ms */);
	}
	kbd_unsubscribe_int();
	return 0;
}

int kbd_test_scan(unsigned short ass) {
	int irq_set = kbd_subscribe_int();
	int ipc_status;
	message msg;
	unsigned char result = 0;
	if (irq_set == ERROR) {
		printf("kbd_test_scan()::kernel call didn't return 0\n");
		return ERROR;
	}
	while (result != KEY_ESC) {
		if (driver_receive(ANY, &msg, &ipc_status) != OK) {
			printf("kbd_test_scan()::driver_receive failed\n");
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					if (ass) {
						result = kbd_int_handler_asm();
					} else {
						result = kbd_int_handler_C();
					}
					if (result == KEY_ESC) {
						printf(
								"kbd_test_scan()::esc was pressed, press any key to continue\n");
					}
				}
				break;
			}
		}
	}
	kbd_unsubscribe_int();
	return 0;
}

int kbd_test_timed_scan(unsigned short n) {
	int ipc_status;
	unsigned char result = 0;
	message msg;
	int kbd_interrupt = kbd_subscribe_int();
	int timer_interrupt = timer_subscribe_int();
	if (kbd_interrupt == ERROR || timer_interrupt == ERROR) {
		printf("kbd_test_timed_scan()::kernel call didn't return 0\n");
		return 1;
	}
	while (result != KEY_ESC && timer_count < n * 60) {
		if (driver_receive(ANY, &msg, &ipc_status) != OK) {
			printf("kbd_test_timed_scan()::driver_receive failed\n");
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & timer_interrupt) {
					timer_int_handler();
					if (timer_count >= n * 60) {
						printf("kbd_test_timed_scan()::timed out\n");
					}
				}
				if (msg.NOTIFY_ARG & kbd_interrupt) {
					result = kbd_int_handler_C();
					if (result == KEY_ESC) {
						printf(
								"kbd_test_timed_scan()::esc was pressed, press any key to continue\n");
					}
				}
				break;
			}
		}
	}
	timer_unsubscribe_int();
	kbd_unsubscribe_int();
	return 0;
}
