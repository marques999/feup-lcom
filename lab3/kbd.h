#ifndef __KBD_H
#define __KBD_H

#include "i8042.h"


/**
 * @brief Reads the next value from the keyboard OUT_BUF
 *
 * @param *data where to save the read value
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_read(unsigned char *data);

/**
 * @brief To test reading of scancode via KBD interrupts
 *
 * Displays the scancodes received from the keyboard
 * Exits upon release of the ESC key
 *
 * @param ass Which IH to use: if 0, in C, otherwise in assembly
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_test_scan(unsigned short ass);


/**
 * @brief To test keyboard commands
 *
 * Toggles LED indicators, one per second, according to the
 *  arguments
 *
 * @param n number of elements of the second argument
 * @param leds array with the LEDs to toggle (Ranges from 0 t0 2)
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_test_leds(unsigned short n, unsigned short *leds);

/**
 * @brief To test handling of more than one interrupt
 *
 *  Similar to kbd_test_scan() except that it
 *  should terminate also if no scancodes are received for n seconds
 *
 * @param n Number of seconds without scancodes before exiting
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_test_timed_scan(unsigned short n);

#endif /* __KBD_H */
