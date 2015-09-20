#ifndef __I8042_H
#define __I8042_H

#define BIT(a) (1 << a)

#define DELAY 20000

/**
 * Mouse specific commands
 */

#define MOUSE_CMD 0xD4
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
 * Mouse-specific bits
 */

#define MOUSE_LB BIT(0)
#define MOUSE_RB BIT(1)
#define MOUSE_MB BIT(2)
#define MOUSE_UNUSED BIT(3)
#define MOUSE_X_SIGN BIT(4)
#define MOUSE_Y_SIGN BIT(5)
#define MOUSE_X_OVF BIT(6)
#define MOUSE_Y_OVF BIT(7)

/**
 * KBC registers
 */

#define STAT_REG 0x64
#define CMD_REG 0x64
#define IN_BUF 0x60
#define OUT_BUF 0x60

#endif /* __I8042_H */
