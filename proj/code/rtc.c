#include <minix/syslib.h>
#include <minix/drivers.h>

#define RTC_IRQ		8
#define RTC_HOOK	4

#include "rtc.h"

static int rtc_hook = RTC_HOOK;

static unsigned long read_bcd(unsigned long i)
{
	i = (((i) >> 4) * 10) + (i & 0x0F);
	return i;
}

static unsigned char write_bcd(unsigned char i)
{
	if (i < 10)
	{
		return i;
	}

	return ((i / 10) << 4) + (i - (i / 10) * 10);
}

__inline static void rtc_read(unsigned long reg, unsigned long* data)
{
	sys_outb(RTC_ADDR_REG, reg);
	sys_inb(RTC_DATA_REG, data);
}

__inline static void rtc_write(unsigned long reg, unsigned long data)
{
	sys_outb(RTC_ADDR_REG, reg);
	sys_outb(RTC_DATA_REG, data);
}

rtc_time_t rtc_current_date()
{
	rtc_time_t new_time;

	unsigned long day = 0;
	unsigned long month = 0;
	unsigned long year = 0;
	unsigned long hours = 0;
	unsigned long minutes = 0;
	unsigned long seconds = 0;

	rtc_read(RTC_SECONDS, &seconds);
	rtc_read(RTC_MINUTES, &minutes);
	rtc_read(RTC_HOURS, &hours);

	rtc_read(RTC_DAY, &day);
	rtc_read(RTC_MONTH, &month);
	rtc_read(RTC_YEAR, &year);

	new_time.seconds = read_bcd(seconds);
	new_time.minutes = read_bcd(minutes);
	new_time.hours = read_bcd(hours);

	new_time.day = read_bcd(day);
	new_time.month = read_bcd(month);
	new_time.year = read_bcd(year);

	return new_time;
}

int rtc_check()
{
    unsigned long r;

    rtc_read(RTC_REG_C, &r);

    return r;
}

int rtc_subscribe(void)
{
	if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hook) != 0)
	{
		return -1;
	}

	if (sys_irqenable(&rtc_hook) != 0)
	{
		return -1;
	}

	return BIT(RTC_HOOK);
}

void rtc_unsubscribe(void)
{
	sys_irqdisable(&rtc_hook);
	sys_irqrmpolicy(&rtc_hook);
}

void rtc_enable()
{
	unsigned long r;

	rtc_check();
	rtc_read(RTC_REG_B, &r);
	rtc_write(RTC_REG_B, (r | (RTC_AIE|RTC_UIE)));
}

void rtc_disable()
{
	unsigned long r;

	rtc_read(RTC_REG_B, &r);
	rtc_write(RTC_REG_B, (r & ~(RTC_AIE|RTC_UIE)));
	rtc_check();
}

void rtc_write_alarm(unsigned short s)
{
	unsigned long r;

	rtc_time_t new_alarm = rtc_current_date();

	new_alarm.seconds += s;
	new_alarm.minutes += new_alarm.seconds / 60;
	new_alarm.seconds %= 60;
	new_alarm.hours += new_alarm.minutes / 60;
	new_alarm.minutes %= 60;

	rtc_read(RTC_REG_B, &r);

	rtc_write(RTC_REG_B, (r | RTC_SET));
	rtc_write(RTC_HOURS_ALARM, write_bcd(new_alarm.hours));
	rtc_write(RTC_MINUTES_ALARM, write_bcd(new_alarm.minutes));
	rtc_write(RTC_SECONDS_ALARM, write_bcd(new_alarm.seconds));
	rtc_write(RTC_REG_B, (r & ~RTC_SET));
}
