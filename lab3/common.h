#ifndef __COMMON_H
#define __COMMON_H

#include <minix/syslib.h>
#include <minix/drivers.h>

#define OK 0
#define ERROR -1

#define BIT(a) (1 << a)

int timer_hook;
int timer_count;

#endif /* __COMMON_H */
