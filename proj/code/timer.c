#include <minix/syslib.h>
#include <minix/drivers.h>

#include "timer.h"

#define TIMER_IRQ	0
#define TIMER_HOOK	0

static int timer_hook = TIMER_HOOK;
static Timer* timer = NULL;

void timer_init(void)
{
	timer = (Timer*) malloc(sizeof(Timer));
	timer->count = 0;
	timer->frame = 0;
	timer->ticked = 0;
}

__inline int timer_current_frame()
{
	return timer->frame;
}

__inline int timer_count()
{
	return timer->count;
}

void timer_handler()
{
	timer->count++;
	timer->frame++;

	if (timer->frame > 60)
	{
		timer->frame %= 60;
	}

	timer->ticked = 1;
}

__inline int timer_ticked()
{
	return timer->ticked;
}

__inline void timer_reset()
{
	timer->ticked = 0;
}

void timer_destroy(void)
{
	if (timer == NULL)
	{
		return;
	}

	free(timer);
	timer = NULL;
}

int timer_subscribe(void)
{
	if (sys_irqsetpolicy(TIMER_IRQ, IRQ_REENABLE, &timer_hook) != 0)
	{
		return -1;
	}

	if (sys_irqenable(&timer_hook) != 0)
	{
		return -1;
	}

	return BIT(TIMER_HOOK);
}

void timer_unsubscribe(void)
{
	sys_irqdisable(&timer_hook);
	sys_irqrmpolicy(&timer_hook);
}
