#ifndef __I8042_H
#define __I8042_H

#define BIT(a) (1 << a)

#define TIMER0_IRQ 0

/**
 * C@KBD command responses
 */
#define ACK 0xfa /**< @brief acknowledged command */
#define RESEND 0xfe /**< @brief resend last command byte */
#define RSP_ERROR 0xfc /**< @brief error sending command */

#define DELAY 20000

#define MOUSE_CMD 0xD4

/**
 * Mouse specific commands
 */
#define STREAM_OFF 0xF5
#define STREAM_ON 0xF4
#define STREAM_ENABLE 0xEA
#define STATUS_REQUEST 0xE9

/**
 * KBC status
 */
#define OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)
#define TO_ERR BIT(5)
#define PAR_ERR BIT(7)

/**
 * KBC registers
 */
#define STAT_REG 0x64
#define CMD_REG 0x64
#define IN_BUF 0x60
#define OUT_BUF 0x60

#endif /* __I8042_H */
