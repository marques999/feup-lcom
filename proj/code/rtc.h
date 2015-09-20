#ifndef __RTC_H
#define __RTC_H

#define BIT(n) (1<<n)

#define RTC_IRQ             8

/**
 * RTC I/O Ports
 */
#define RTC_ADDR_REG        0x70
#define RTC_DATA_REG        0x71

/**
 * RTC Registers
 */
#define RTC_SECONDS			0x0
#define RTC_SECONDS_ALARM	0x1
#define RTC_MINUTES			0x2
#define RTC_MINUTES_ALARM	0x3
#define RTC_HOURS			0x4
#define RTC_HOURS_ALARM		0x5
#define RTC_DAY				0x7
#define RTC_MONTH			0x8
#define RTC_YEAR			0x9
#define RTC_REG_A			0xA
#define RTC_REG_B			0xB
#define RTC_REG_C			0xC
#define RTC_REG_D			0xD

/*
 * RTC_REG_A BITS
 */
#define RTC_RS0			BIT(0)
#define RTC_RS1			BIT(1)
#define RTC_RS2			BIT(2)
#define RTC_RS3			BIT(3)
#define RTC_DV0			BIT(4)
#define RTC_DV1			BIT(5)
#define RTC_DV2			BIT(6)
#define RTC_UIP			BIT(7)

/*
 * RTC_REG_B BITS
 */
#define RTC_DSE			BIT(0)
#define RTC_MODE		BIT(1)
#define RTC_DM			BIT(2)
#define RTC_SQWE		BIT(3)
#define RTC_UIE			BIT(4)
#define RTC_AIE			BIT(5)
#define RTC_PIE			BIT(6)
#define RTC_SET			BIT(7)

/*
 * RTC_REG_C BITS
 */
#define RTC_UF			BIT(4)
#define RTC_AF			BIT(5)
#define RTC_PF			BIT(6)
#define RTC_IRQF		BIT(7)

/**
 * RTC_REG_D BITS
 */
#define RTC_VRT_BIT		BIT(7)

typedef struct
{
	unsigned long seconds;
	unsigned long minutes;
	unsigned long hours;

	unsigned long day;
	unsigned long month;
	unsigned long year;

} rtc_time_t;

/**
 * @brief reads system local time from the real-time clock
 * @return returns a rtc_time_t struct containing the read values
 */
rtc_time_t rtc_current_date();

/**
 * @brief reads real-time clock register C status
 * @return returns the read value (interrupt mask)
 */
int rtc_check();

/**
 * @brief subscribes real-time clock interrupts
 * @return returns bit order in interrupt mask; negative value on failure
 */
int rtc_subscribe();

/**
 * @brief unsubscribes real-time clock interrupts
 */
void rtc_unsubscribe();

/**
 * @brief enables real-time clock alarm and update interrupts
 */
void rtc_enable();

/**
 * @brief disables real-time clock alarm and update interrupts
 */
void rtc_disable();

/**
 * @brief writes an alarm to the real time clock
 * @param s alarm seconds
 */
void rtc_write_alarm(unsigned short s);

#endif /* __RTC_H */
