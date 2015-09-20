#ifndef __TIMER_H
#define __TIMER_H

#define BIT(n) (1<<n)

#define TIMER_FREQ  1193182

/*
 *  I/O Port Addresses
 */
#define TIMER_0			0x40
#define TIMER_1			0x41
#define TIMER_2			0x42
#define TIMER_CTRL		0x43

/*
 * Timer Control Register
 */
#define TIMER_SEL0		0
#define TIMER_SEL1		BIT(6)
#define TIMER_SEL2		BIT(7)
#define TIMER_RB_CMD	(BIT(7)|BIT(6))
#define TIMER_LSB		BIT(4)
#define TIMER_MSB		BIT(5)
#define TIMER_LSB_MSB	(TIMER_LSB | TIMER_MSB)
#define TIMER_SQR_WAVE	(BIT(2)|BIT(1))
#define TIMER_RATE_GEN	BIT(2)
#define TIMER_BCD		BIT(0)
#define TIMER_BIN		0

typedef struct
{
	int frame;
	int count;
	int ticked;
} Timer;

/**
 * @brief subscribes and enables Timer 0 interrupts
 * @return returns bit order in interrupt mask; negative value on failure
 */
int timer_subscribe(void);

/**
 * @brief unsubscribes Timer 0 interrupts
 * @return return 0 upon success and non-zero otherwise
 */
void timer_unsubscribe(void);

/**
 * @brief timer 0 interrupt handler
 */
void timer_handler(void);

/**
 * @brief creates a new timer instance
 */
Timer* timer_create(void);

/**
 * @brief frees the memory allocated by the pointer
 */
void timer_destroy(void);

/**
 * @brief returns the number of interrupts processed by the timer
 */
int timer_count();

/**
 * @brief returns the current frame (reset every VBLANK)
 */
int timer_current_frame();

#endif /* __TIMER_H */
