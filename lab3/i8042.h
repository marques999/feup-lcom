#ifndef __I8042_H
#define __I8042_H

#define BIT(a) (1 << a)

/**
 * C@KBD command responses
 */
#define RSP_ACK 0xfa /**< @brief acknowledged command */
#define RSP_RESEND 0xfe /**< @brief resend last command byte */
#define RSP_ERROR 0xfc /**< @brief error sending command */

/**
 * LED related defines and macros
 */
#define CMD_LED 0xED
#define KBD_LED(a) BIT(a)
#define SCROLL_LOCK BIT(0)
#define NUM_LOCK BIT(1)
#define CAPS_LOCK BIT(2)

/////
#define TWO_BYTE 0xe0
#define KEY_ESC 0x81

//
#define DELAY 20000

#define OBF BIT(0)
#define IBF BIT(1)
#define TO_ERR BIT(5)
#define PAR_ERR BIT(7)

#define BREAK_BIT BIT(7)
#define is_break_code(a) (a & BREAK_BIT)

/**
 * KBC registers
 */
#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define IN_BUF 0x60
#define OUT_BUF 0x60

#endif /* __I8042_H */
