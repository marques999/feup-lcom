#ifndef __TIMER_H
#define __TIMER_H

#include "common.h"

/** @defgroup timer timer
 * @{
 *
 * Functions for using the i8254 timers
 */


/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int timer_subscribe_int();

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
void timer_int_handler();

void timer_wait(unsigned int ticks);

#endif /* __TIMER_H */
