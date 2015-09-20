#include "timer.h"
#include "i8042.h"

int timer_hook = 1;

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
