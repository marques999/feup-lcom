#include <minix/syslib.h>
#include <minix/drivers.h>

#include "input.h"
#include "serial.h"

#define COM1_IRQ 4
#define COM2_IRQ 3

#define COM1_PORT 0x3F8
#define COM2_PORT 0x2F8

#define SERIAL_HOOK 3

static int serial_hook = SERIAL_HOOK;

void serial_write_char(unsigned char chr)
{
	while (!(is_thr_empty()))
	{
	}

	sys_outb(COM1_PORT + TH_REG, chr);
	tickdelay(micros_to_ticks(UART_DELAY));
}

unsigned char serial_read_char()
{
	unsigned long status = 0;

	sys_inb(COM1_PORT + LS_REG, &status);

	unsigned long r = 0;

	if (status & SERIAL_RECIEVED_DATA)
	{
		sys_inb(COM1_PORT + RB_REG, &r);

		return (unsigned char)(r & 0xFF);
	}

	return 0;
}

void serial_write_bitrate(unsigned long br)
{
	unsigned long f = SERIAL_BASE_FREQ / br;
	unsigned long msb = (f >> 8) & 0xFF;
	unsigned long lsb = f & 0xFF;
	unsigned long r = 0;

	sys_inb(COM1_PORT + LC_REG, &r);

	r |= DLAB;

	sys_outb(COM1_PORT + LC_REG, r);
	sys_outb(COM1_PORT + DLM_REG, msb);
	sys_outb(COM1_PORT + DLL_REG, lsb);

	r ^= DLAB;

	sys_outb(COM1_PORT + LC_REG, r);
}

int is_thr_empty()
{
	unsigned long r = 0;

	sys_inb(COM1_PORT + LS_REG, &r);

	return (r & SERIAL_THR_EMPTY) ? 1 : 0;
}

#define PADDLE_LEFT 			BIT(1)
#define PADDLE_LEFT_BREAK		BIT(6)|BIT(1)
#define PADDLE_RIGHT			BIT(2)
#define PADDLE_RIGHT_BREAK		BIT(6)|BIT(2)
#define PADDLE_LB				BIT(3)
#define PADDLE_LB_BREAK			BIT(6)|BIT(3)
#define PADDLE_MB				BIT(4)
#define PADDLE_MB_BREAK 		BIT(6)|BIT(4)
#define PADDLE_RB				BIT(5)
#define PADDLE_RB_BREAK			BIT(6)|BIT(5)

void serial_send_paddle(keycode_t scancode)
{
	switch (scancode)
	{
	case KEY_A:
		serial_write_char((unsigned char) PADDLE_LEFT);
		break;
	case (KEY_A | 0x80):
		serial_write_char((unsigned char) PADDLE_LEFT_BREAK);
		break;
	case KEY_D:
		serial_write_char((unsigned char) PADDLE_RIGHT);
		break;
	case (KEY_D | 0x80):
		serial_write_char((unsigned char) PADDLE_RIGHT_BREAK);
		break;
	case KEY_ENTER:
		serial_write_char((unsigned char) PADDLE_LB);
		break;
	case (KEY_ENTER | 0x80):
		serial_write_char((unsigned char) PADDLE_LB_BREAK);
		break;
	case KEY_ESC:
		serial_write_char((unsigned char) PADDLE_RB);
		break;
	case (KEY_ESC | 0x80):
		serial_write_char((unsigned char) PADDLE_RB_BREAK);
		break;
	}
}

unsigned long serial_recieve_paddle(key_t scancode)
{
	if (scancode == 0x55)
	{
		return scancode;
	}

	unsigned long r;

	switch (scancode)
	{
	case PADDLE_LEFT:
		r = KEY_A;
		break;
	case PADDLE_LEFT_BREAK:
		r = KEY_A | 0x80;
		break;
	case PADDLE_RIGHT:
		r = KEY_D;
		break;
	case PADDLE_RIGHT_BREAK:
		r = KEY_D | 0x80;
		break;
	case PADDLE_LB:
		r = KEY_ENTER;
		break;
	case PADDLE_LB_BREAK:
		r = KEY_ENTER | 0x80;
		break;
	case PADDLE_RB:
		r = KEY_ESC;
		break;
	case PADDLE_RB_BREAK:
		r = KEY_ESC | 0x80;
		break;
	}

	return r;
}

void serial_enable()
{
	unsigned long r = 0;

	sys_inb(COM1_PORT + IE_REG, &r);

	r |= (ENABLE_RDA_INT | ENABLE_LSR_INT);

	sys_outb(COM1_PORT + IE_REG, r);

	tickdelay(micros_to_ticks(UART_DELAY));
}

void serial_disable()
{
	unsigned long r = 0;

	sys_inb(COM1_PORT + IE_REG, &r);

	r &= DISABLE_RDA_INT;
	r &= DISABLE_LSR_INT;

	sys_outb(COM1_PORT + IE_REG, r);

	tickdelay(micros_to_ticks(UART_DELAY));
}

int serial_subscribe()
{
	if (sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &serial_hook) != 0)
	{
		return -1;
	}

	if (sys_irqenable(&serial_hook) != 0)
	{
		return -1;
	}

	return BIT(SERIAL_HOOK);
}

int serial_unsubscribe()
{
	if (sys_irqdisable(&serial_hook) != 0)
	{
		return -1;
	}

	if (sys_irqrmpolicy(&serial_hook) != 0)
	{
		return -1;
	}

	return 0;
}

int serial_init()
{
	serial_disable();

	unsigned long r;

	sys_inb(COM1_PORT + LC_REG, &r);

	r &= 0x40;
	r |= 0x80;
	r |= CHAR_EIGHT_BITS;
	r |= ONE_STOP_BIT;
	r |= PARITY_NONE;

	sys_outb(COM1_PORT + LC_REG, r);

	serial_write_bitrate(SERIAL_BITRATE);

	return serial_subscribe();
}
