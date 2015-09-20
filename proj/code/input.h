#ifndef __MOUSE_H
#define __MOUSE_H

#include "bitmap.h"
#include "video.h"

#define BIT(n) (1<<n)

/**
 * Mouse specific commands
 */
#define MOUSE_CMD 0xD4
#define STREAM_OFF 0xF5
#define STREAM_ON 0xF4
#define STREAM_ENABLE 0xEA
#define STATUS_REQUEST 0xE9

/**
 * Mouse status register bits
 */
#define MOUSE_LB BIT(0)
#define MOUSE_RB BIT(1)
#define MOUSE_MB BIT(2)
#define MOUSE_UNUSED BIT(3)
#define MOUSE_X_SIGN BIT(4)
#define MOUSE_Y_SIGN BIT(5)
#define MOUSE_X_OVF BIT(6)
#define MOUSE_Y_OVF BIT(7)

#define DELAY 20000

/**
 * C@KBD command responses
 */
#define ACK 0xfa
#define RESEND 0xfe
#define RSP_ERROR 0xfc

/**
 * KBC status
 */
#define OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)
#define TO_ERR BIT(5)
#define PAR_ERR BIT(7)

/**
 * I8042(KBC) registers
 */
#define STAT_REG 0x64
#define CMD_REG 0x64
#define IN_BUF 0x60
#define OUT_BUF 0x60
#define TWO_BYTE 0xe0

/**
 * keyboard scancodes enumerator
 */
typedef enum
{
    KEY_NONE = 0x0000,
    KEY_ESC = 0x0001,
    KEY_1 = 0x0002,
    KEY_2 = 0x0003,
    KEY_3 = 0x0004,
    KEY_4 = 0x0005,
    KEY_5 = 0x0006,
    KEY_6 = 0x0007,
    KEY_7 = 0x0008,
    KEY_8 = 0x0009,
    KEY_9 = 0x000A,
    KEY_0 = 0x000B,
    KEY_APOSTROPHE = 0x000C,
    KEY_ANGLE_QUOTES = 0x000D,
    KEY_BKSP = 0x000E,
    KEY_TAB = 0x000F,
    KEY_Q = 0x0010,
    KEY_W = 0x0011,
    KEY_E = 0x0012,
    KEY_R = 0x0013,
    KEY_T = 0x0014,
    KEY_Y = 0x0015,
    KEY_U = 0x0016,
    KEY_I = 0x0017,
    KEY_O = 0x0018,
    KEY_P = 0x0019,
    KEY_PLUS = 0x001A,
    KEY_ACCENT = 0x001B,
    KEY_ENTER = 0x001C,
    KEY_L_CTRL = 0x001D,
    KEY_A = 0x001E,
    KEY_S = 0x001F,
    KEY_D = 0x0020,
    KEY_F = 0x0021,
    KEY_G = 0x0022,
    KEY_H = 0x0023,
    KEY_J = 0x0024,
    KEY_K = 0x0025,
    KEY_L = 0x0026,
    KEY_C_CEDILLA = 0x0027,
    KEY_ORDINAL = 0x0028,
    KEY_BACKSLASH = 0x0029,
    KEY_L_SHIFT = 0x002A,
    KEY_TILDE = 0x002B,
    KEY_Z = 0x002C,
    KEY_X = 0x002D,
    KEY_C = 0x002E,
    KEY_V = 0x002F,
    KEY_B = 0x0030,
    KEY_N = 0x0031,
    KEY_M = 0x0032,
    KEY_COMMA = 0x0033,
    KEY_DOT = 0x0034,
    KEY_MINUS = 0x0035,
    KEY_R_SHIFT = 0x0036,
    KEY_ALT = 0x0038,
    KEY_SPACE = 0x0039,
    KEY_CAPS = 0x003A,
    KEY_F1 = 0x003B,
    KEY_F2 = 0x003C,
    KEY_F3 = 0x003D,
    KEY_F4 = 0x003E,
    KEY_F5 = 0x003F,
    KEY_F6 = 0x0040,
    KEY_F7 = 0x0041,
    KEY_F8 = 0x0042,
    KEY_F9 = 0x0043,
    KEY_F10 = 0x0044,
    KEY_NUM = 0x0045,
    KEY_SCRLL = 0x0046,
    KEY_NUM_7 = 0x0047,
    KEY_NUM_8 = 0x0048,
    KEY_NUM_9 = 0x0049,
    KEY_NUM_MINUS = 0x004A,
    KEY_NUM_4 = 0x004B,
    KEY_NUM_5 = 0x004C,
    KEY_NUM_6 = 0x004D,
    KEY_NUM_PLUS = 0x004E,
    KEY_NUM_1 = 0x004F,
    KEY_NUM_2 = 0x0050,
    KEY_NUM_3 = 0x0051,
    KEY_NUM_0 = 0x0052,
    KEY_NUM_DEL = 0x0053,
    KEY_MINOR = 0x0056,
    KEY_F11 = 0x0057,
    KEY_F12 = 0x0058,
    KEY_NUM_ENTER = 0xE01C,
    KEY_R_CTRL = 0xE01D,
    KEY_NUM_SLASH = 0xE035,
    KEY_ALT_GR = 0xE038,
    KEY_HOME = 0xE047,
    KEY_ARR_UP = 0xE048,
    KEY_PGUP = 0xE049,
    KEY_ARR_LEFT = 0xE04B,
    KEY_ARR_RIGHT = 0xE04D,
    KEY_ARR_DOWN = 0xE050,
    KEY_PGDN = 0xE051,
    KEY_INS = 0xE052,
    KEY_DEL = 0xE053,
    KEY_WIN = 0xE05B,
    KEY_CNTX = 0xE05D,
    KEY_END = 0xE04F,
} keycode_t;

/**
 * @brief mouse operating modes enumerator
 */
typedef enum {
	PADDLE, CURSOR
} mmode_t;

/**
 * @brief mouse data structure
 */
typedef struct {

	uint8_t packet[3]; ///> mouse information packets (3 bytes)
	int packet_count; ///> current mouse packet array index

	bitmap_t* cursor; ///> cursor bitmap

	int width; ///> cursor bitmap width
	int height; ///> cursor bitmap height

	int x; ///> cursor x (horizontal) position
	int y; ///> cursor y (vertical) position
	int xspeed; ///> cursor x (horizontal) instantaneous velocity
	int yspeed; ///> cursor y (vertical) instantaneous velocity

	int updated; ///> indicates if mouse has been updated

	unsigned leftButton; ///> left button status (pressed)
	unsigned leftButtonReleased; ///> left button status (released)
	unsigned middleButton; ///> middle button status (pressed)
	unsigned middleButtonReleased; ///> middle button status (released)
	unsigned rightButton; ///> right button status (pressed)
	unsigned rightButtonReleased; ///> right button status (released)

	mmode_t mode; ///> current mouse operating mode

} mouse_t;

/**
 * @brief creates a new instance of mouse_t
 * @param filename absolute path to the mouse cursor bitmap
 * @param mt mouse operating mode (cursor or paddle)
 * @param x initial mouse position on the x axis (horizontal)
 * @param y initial mouse position on the y axis (vertical)
 * @return returns the newly created mouse structure
 */
mouse_t* mouse_create(const char* filename, mmode_t mt, unsigned x, unsigned y);

/**
 * @brief updates mouse position on screen and button status
 * @param mouse instance of mouse data structure
 * @param bounds rectangle that contains the screen bounds
 */
void mouse_update(mouse_t* mouse, rectangle_t* bounds);

/**
 * @brief destroys and frees memory allocated by a mouse_t object
 * @param mouse instance of mouse data structure
 */
void mouse_destroy(mouse_t* mouse);

/**
 * @brief draws mouse cursor
 * @param mouse instance of mouse data structure
 * @param buffer graphics buffer where the cursor is drawn
 */
void mouse_draw(mouse_t* mouse, short* buffer);

/**
 * @brief handles mouse interrupts
 * @param mouse instance of mouse data structure
 * @param bounds rectangle that contains the screen bounds
 */
void mouse_handler(mouse_t* mouse, rectangle_t* bounds);

/**
 * @brief checks if mouse is inside a rectangle on screen
 * @param mouse instance of mouse data structure
 * @param rect rectangle that contains the area of the screen to be checked
 * @return returns 1 if conditions are true; zero otherwise
 */
int mouse_inside(mouse_t* mouse, rectangle_t* rect);

/**
 * @brief subscribes mouse interrupts
 * @return returns bit order in interrupt mask; negative value on failure
 */
int mouse_subscribe();

/**
 * @brief unsubscribes mouse interrupts
 * @return return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe();

/**
 * @brief handles keyboard interrupts
 * @return returns the processed keyboard scancode
 * @warning only 1-byte and 2-byte keyboard scancodes are supported!
 */
unsigned short kbd_handler();

/**
 * @brief subscribes keyboard interrupts
 * @return returns bit order in interrupt mask; negative value on failure
 */
int kbd_subscribe();

/**
 * @brief unsubscribes keyboard interrupts
 * @return returns 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe();

#endif
