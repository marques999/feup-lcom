#include <minix/syslib.h>
#include <minix/drivers.h>

#include "input.h"

#define KBD_HOOK		1
#define KBD_IRQ 		1

#define MOUSE_HOOK 		2
#define MOUSE_IRQ 		12

#define MAX_TRIES 		4

#define SIGN_EXTEND 0xffffff00
#define ZERO_EXTEND 0x000000ff

static int kbd_hook = KBD_HOOK;
static int mouse_hook = MOUSE_HOOK;

extern int kbd_read();

#define clamp(num, min, max) ((num < min) ? min : ((num > max) ? max : num))
#define mouse_position(value, sign) (sign ? ((short) value | SIGN_EXTEND) : ((short) value & ZERO_EXTEND))

__inline int mouse_inside(mouse_t* mouse, rectangle_t* rect)
{
	return (rect->xi <= mouse->x && mouse->x <= rect->xf && rect->yi <= mouse->y && mouse->y <= rect->yf);
}

mouse_t* mouse_create(const char* filename, mmode_t mt, unsigned x, unsigned y)
{
	mouse_t* new_mouse = (mouse_t*) malloc(sizeof(mouse_t));

	new_mouse->cursor = (bitmap_t*) bitmap_read(filename);
	new_mouse->x = x;
	new_mouse->y = y;
	new_mouse->xspeed = 0;
	new_mouse->yspeed = 0;
	new_mouse->width = bitmap_width(new_mouse->cursor);
	new_mouse->height = bitmap_height(new_mouse->cursor);
	new_mouse->packet_count = 0;
	new_mouse->mode = mt;
	new_mouse->leftButton = 0;
	new_mouse->leftButtonReleased = 0;
	new_mouse->middleButton = 0;
	new_mouse->middleButtonReleased = 0;
	new_mouse->rightButton = 0;
	new_mouse->rightButtonReleased = 0;

	return new_mouse;
}

static void mouse_reset(mouse_t* mouse)
{
	mouse->leftButton = 0;
	mouse->middleButton = 0;
	mouse->rightButton = 0;
}

/**
 * @brief updates mouse button status using mouse packetss
 * @param mouse instance of mouse data structure
 */
void mouse_update_buttons(mouse_t* mouse)
{
	uint8_t leftButton = mouse->packet[0] & MOUSE_LB ? 1 : 0;

	if (leftButton == 0 && mouse->leftButton == 1)
	{
		mouse->leftButtonReleased = 1;
		mouse->leftButton = 0;
	}
	else
	{
		mouse->leftButtonReleased = 0;
		mouse->leftButton = leftButton;
	}

	uint8_t middleButton = mouse->packet[0] & MOUSE_MB ? 1 : 0;

	if (!middleButton && mouse->middleButton)
	{
		mouse->middleButtonReleased = 1;
		mouse->middleButton = 0;
	}
	else
	{
		mouse->middleButtonReleased = 0;
		mouse->middleButton = middleButton;
	}

	uint8_t rightButton = mouse->packet[0] & MOUSE_RB ? 1 : 0;

	if (!rightButton && mouse->rightButton)
	{
		mouse->rightButtonReleased = 1;
		mouse->rightButton = 0;
	}
	else
	{
		mouse->rightButtonReleased = 0;
		mouse->rightButton = rightButton;
	}
}

void mouse_update(mouse_t* mouse, rectangle_t* bounds)
{
	uint8_t signX = mouse->packet[0] & MOUSE_X_SIGN ? 1 : 0;
	int16_t posX = mouse_position(mouse->packet[1], signX);
	uint8_t signY = mouse->packet[0] & MOUSE_Y_SIGN ? 1 : 0;
	int16_t posY = mouse_position(mouse->packet[2], signY);

	mouse->updated = 0;

	mouse_update_buttons(mouse);

	mouse->xspeed = posX;
	mouse->yspeed = posY;
	mouse->x += posX;
	mouse->y -= posY;
	mouse->x = clamp(mouse->x, bounds->xi, bounds->xf);
	mouse->y = clamp(mouse->y, bounds->yi, bounds->yf);

	if (posX =! 0 || posY != 0)
	{
		mouse->updated = 1;
	}
}

/**
 * @brief updates mouse position on screen in paddle mode using mouse packets
 * @param paddle instance of mouse data structure in paddle mode
 * @param bounds rectangle that contains the playfield bounds
 */
static void paddle_mouse_update(mouse_t* paddle, rectangle_t* bounds)
{
	uint8_t signX = paddle->packet[0] & MOUSE_X_SIGN ? 1 : 0;
	int16_t posX = mouse_position(paddle->packet[1], signX);

	paddle->updated = 0;

	mouse_update_buttons(paddle);

	paddle->xspeed = posX;
	paddle->yspeed = 0;
	paddle->x += posX;
	paddle->y = 700;
	paddle->x = clamp(paddle->x, bounds->xi, bounds->xf - paddle->width);

	if (posX != 0)
	{
		paddle->updated = 1;
	}
}

/**
 * @brief updates mouse position on screen in padddle mode using keyboard packets
 * @param paddle instance of mouse data structure in paddle mode
 * @param bounds rectangle that contains the playfield bounds
 */
void paddle_keyboard_refresh(mouse_t* paddle, rectangle_t* bounds)
{
	paddle->x += paddle->xspeed;
	paddle->x = clamp(paddle->x, bounds->xi, bounds->xf - paddle->width);
	paddle->y = 700;
}

/**
 * @brief updates mouse button status using keyboard packets
 * @param mouse instance of mouse data structure
 * @param scancode keyboard scancode returned by keyboard handler
 */
void keyboard_update_buttons(mouse_t* mouse, key_t scancode)
{
	switch (scancode)
	{
	case KEY_ENTER:
		mouse->leftButton = 1;
		break;
	case (KEY_ENTER | 0x80):
		mouse->leftButton = 0;
		break;
	case KEY_SPACE:
		mouse->middleButton = 1;
		break;
	case (KEY_SPACE | 0x80):
		mouse->middleButton = 0;
		break;
	case KEY_ESC:
		mouse->rightButton = 1;
		break;
	case (KEY_ESC | 0x80):
		mouse->rightButton = 0;
		break;
	}
}

/**
 * @brief updates mouse speed in padddle mode using keyboard packets
 * @param paddle instance of mouse data structure in paddle mode
 * @param bounds rectangle that contains the playfield bounds
 * @param scancode keyboard scancode returned by the keyboard handler
 */
void paddle_keyboard_update(mouse_t* paddle, rectangle_t* bounds, key_t scancode)
{
	paddle->updated = 0;

	keyboard_update_buttons(paddle, scancode);

	switch (scancode)
	{
	case KEY_A:
	{
		paddle->xspeed = -12;
		paddle->updated = 1;
		break;
	}
	case (KEY_A | 0x80):
	{
		if (paddle->xspeed == -12)
		{
			paddle->xspeed = 0;
			paddle->updated = 1;
		}
		break;
	}
	case KEY_D:
	{
		paddle->xspeed = 12;
		paddle->updated = 1;
		break;
	}
	case (KEY_D | 0x80):
	{
		if (paddle->xspeed == 12)
		{
			paddle->xspeed = 0;
			paddle->updated = 1;
		}
		break;
	}
	}
}

void mouse_destroy(mouse_t* mouse)
{
	if (mouse == NULL)
	{
		return;
	}

	free(mouse->cursor);
	free(mouse);

	mouse = NULL;
}

void mouse_draw(mouse_t* mouse, short* buffer)
{
	int x, y;
	int i = 0;

	for (y = mouse->y + mouse->height - 1; y >= mouse->y; y--)
	{
		for (x = mouse->x; x < mouse->x + mouse->width; x++)
		{
			if (mouse->cursor->bitmapData[i] != (short) 0xF81F)
			{
				draw_pixel(x, y, mouse->cursor->bitmapData[i], buffer);
			}

			i++;
		}

		if (mouse->width % 2)
		{
			i++;
		}
	}
}

/**
 * @brief reads mouse packets from the keyboard controller
 * @param data char pointer where the data is going to be saved
 * @return returns 0 in case of success, 1 otherwise
 */
static int mouse_read(unsigned char *data)
{
	unsigned char status = 0;

	sys_inb(STAT_REG, (unsigned long *) &status);

	if (status & (OBF|AUX))
	{
		if (sys_inb(OUT_BUF, (unsigned long *) data) != OK)
		{
			return 1;
		}

		return 0;
	}
}

/**
 * @brief writes mouse command to the keyboard controller
 * @param data contains the command or the argument to be sent
 * @return returns controller response in case of success, 1 otherwise
 */
static int mouse_write(unsigned char data)
{
	unsigned char status = 0;

	sys_inb(STAT_REG, (unsigned long *) &status);

	if ((status & IBF) == 0)
	{
		if (sys_outb(CMD_REG, MOUSE_CMD) != 0)
		{
			return 1;
		}

		if (sys_outb(IN_BUF, data) != 0)
		{
			return 1;
		}

		tickdelay(micros_to_ticks(DELAY));
		mouse_read(&status);

		return status;
	}
}

unsigned short kbd_handler()
{
	unsigned short r = kbd_read();

	if (r == TWO_BYTE)
	{
		return (r | kbd_read());
	}

	return r;
}

int kbd_subscribe()
{
	int r = BIT(kbd_hook);

	if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook) != 0)
	{
		return -1;
	}

	if (sys_irqenable(&kbd_hook) != 0)
	{
		return -1;
	}

	return r;
}

int mouse_subscribe()
{
	int r = BIT(mouse_hook);

	mouse_write(STREAM_OFF);

	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook) != 0)
	{
		return -1;
	}

	if (sys_irqenable(&mouse_hook) != 0)
	{
		return -1;
	}

	mouse_write(STREAM_ENABLE);
	mouse_write(STREAM_ON);

	unsigned long in = 0;
	sys_inb(OUT_BUF, &in);

	return r;
}

int mouse_unsubscribe()
{
	if (sys_irqdisable(&mouse_hook) != 0)
	{
		return 1;
	}

	if (sys_irqrmpolicy(&mouse_hook) != 0)
	{
		return 1;
	}

	mouse_write(STREAM_OFF);

	return OK;
}

int kbd_unsubscribe()
{
	unsigned long in = 0;

	sys_inb(OUT_BUF, &in);

	if (sys_irqdisable(&kbd_hook) != OK)
	{
		return 1;
	}

	if (sys_irqrmpolicy(&kbd_hook) != OK)
	{
		return 1;
	}

	return OK;
}

void mouse_handler(mouse_t* mouse, rectangle_t* bounds)
{
	unsigned char r = 0;

	if (mouse_read(&r) != OK)
	{
		return;
	}
	if (mouse->packet_count == 0)
	{
		if (r & MOUSE_UNUSED)
		{
			mouse->packet[mouse->packet_count] = r;
			mouse->packet_count++;
		}
	}
	else
	{
		mouse->packet[mouse->packet_count++] = r;

		if (mouse->packet_count == 3)
		{
			mouse->packet_count = 0;

			if (mouse->mode == PADDLE)
			{
				paddle_mouse_update(mouse, bounds);
			}
			else
			{
				mouse_update(mouse, bounds);
			}
		}
	}
}
