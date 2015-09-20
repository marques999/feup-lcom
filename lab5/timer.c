#include "timer.h"

#define TIMER0_HOOK_ID	1
#define TIMER0_IRQ		0

static int timer_hook = TIMER0_HOOK_ID;

int timer_subscribe_int() {
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook) != OK) {
		return ERROR;
	}
	if (sys_irqenable(&timer_hook) != OK) {
		return ERROR;
	}
	return BIT(TIMER0_HOOK_ID);
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

void timer_wait(unsigned int ticks) {
	int ipc_status;
	int timer_count = 0;
	message msg;
	short timer_interrupt = timer_subscribe_int();
	while (timer_count < ticks) {
		if (driver_receive(ANY, &msg, &ipc_status) != OK) {
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & timer_interrupt) {
					timer_count++;
				}
				break;
			}
		}
	}
	timer_unsubscribe_int();
}
