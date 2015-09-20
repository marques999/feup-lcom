#ifndef __SERIAL_H
#define __SERIAL_H

#define BIT(n) (1<<n)


#define COM1_PORT 0x3F8
#define COM2_PORT 0x2F8

#define UART_DELAY			10000

#define SERIAL_BITRATE		1200
#define SERIAL_BASE_FREQ	115200

#define SERIAL_RECIEVED_DATA	BIT(0)
#define SERIAL_RX_RDY			BIT(0)
#define SERIAL_ERROR_OVERRUN	BIT(1)
#define SERIAL_ERROR_PARITY		BIT(2)
#define SERIAL_ERROR_FRAME		BIT(3)
#define SERIAL_ERROR_BREAK		BIT(4)
#define SERIAL_THR_EMPTY		BIT(5)
#define SERIAL_TX_RDY			BIT(5)
#define SERIAL_TR_EMPTY			BIT(6)
#define SERIAL_ERROR_FIFO		BIT(7)


#define SERIAL_LSR_INT		BIT(2)|BIT(1)
#define SERIAL_RX_INT		BIT(2)
#define SERIAL_TX_INT		BIT(1)
#define SERIAL_NO_INT		BIT(0)

#define ENABLE_RDA_INT		BIT(0) ///> enables Recieved Data Available interrupts
#define DISABLE_RDA_INT		(~ENABLE_RDA_INT) ///> disables Recieved Data Available interrupts

#define ENABLE_THR_INT		BIT(1) ///> enables Transmission Holding Register Empty interrupts
#define DISABLE_THR_INT		(~ENABLE_THR_INT) ///> disables Transmission Holding Register Empty interrupts

#define ENABLE_LSR_INT		BIT(2) ///> enables Reciever Line Status interrupts
#define DISABLE_LSR_INT		(~ENABLE_LSR_INT) ///> disables Reciever Line Status interrupts

#define ENABLE_MSR_INT		BIT(3) ///> enables MODEM Status interrupts
#define DISABLE_MSR_INT		(~ENABLE_MSR_INT) ///> disables MODEM Status interrupts

#define RB_REG			0x00
#define TH_REG			0x00
#define DLL_REG			0x00
#define IE_REG			0x01
#define DLM_REG			0x01
#define II_REG 			0x02
#define LC_REG			0x03
#define MC_REG			0x04
#define LS_REG			0x05
#define MS_REG			0x06
#define SP_REG			0x07

#define CHAR_FIVE_BITS		0
#define CHAR_SIX_BITS		BIT(0)
#define CHAR_SEVEN_BITS		BIT(1)
#define CHAR_EIGHT_BITS		(BIT(1)|BIT(0))

#define ONE_STOP_BIT	0
#define TWO_STOP_BITS	BIT(2)

#define PARITY_NONE			0
#define PARITY_ODD			BIT(3)
#define PARITY_EVEN			(BIT(4)|BIT(3))
#define PARITY_ALWAYS1		(BIT(5)|BIT(3))
#define PARITY_ALWAYS0		(BIT(5)|BIT(4)|BIT(3))

#define DLAB				BIT(7)

/**
 * @brief enables Recieved Data Available and Line Status Register interrupts
 */
void serial_enable();

/**
 * @brief disables Recieved Data Available and Line Status Register interrupts
 */
void serial_disable();

/**
 * @brief configures defaults for serial port communication
 * @return returns bit order in interrupt mask (same as subscribe)
 */
int serial_init();

/**
 * @brief subscribes serial port (COM1) interrupts
 * @return returns bit order in interrupt mask; negative value on failure
 */
int serial_subscribe();

/**
 * @brief unsubscribes serial port (COM1) interrupts
 */
int serial_unsubscribe();

#endif
