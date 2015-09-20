#ifndef __TIMER_H
#define __TIMER_H

#include <minix/syslib.h>
#include <minix/drivers.h>

#define OK 0
#define ERROR -1

int timer_hook;
int timer_count;
int timer_subscribe_int();
int timer_unsubscribe_int();
void timer_int_handler();

#endif /* __TIMER_H */
