#include "timer.h"
#include "i8254.h"

int timer_hook = 4;

int timer_subscribe_int() {
	int bit_order = timer_hook;
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook) != OK) {
		return ERROR;
	}
	if (sys_irqenable(&timer_hook) != OK) {
		return ERROR;
	}
	return BIT(bit_order);
}

int timer_unsubscribe_int() {
	if (sys_irqdisable(&timer_hook) != OK) {
		return ERROR;
	}
	if (sys_irqrmpolicy(&timer_hook) != OK) {
		return ERROR;
	}
	return OK;
}

void timer_int_handler() {
	timer_count++;
}

void timer_wait(unsigned int ticks) {
	int ipc_status;
	message msg;
	timer_count = 0;
	short timer_interrupt = timer_subscribe_int();
	while (timer_count < ticks) {
		if (driver_receive(ANY, &msg, &ipc_status) != OK) {
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & timer_interrupt) {
					timer_int_handler();
				}
				break;
			}
		}
	}
	timer_unsubscribe_int();
}
