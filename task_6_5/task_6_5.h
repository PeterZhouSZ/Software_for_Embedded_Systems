#ifndef _TASK_6_5_H_
#define _TASK_6_5_H_

/* INCLUDES ***********************************************************/
#include "ses_common.h"
#include "ses_uart.h"
#include "ses_button.h"
#include "ses_led.h"
#include <avr/boot.h>
#include <avr/pgmspace.h>

/* FUNCTION PROTOTYPES ************************************************/
/**
 * Read from memory block
 *
 * @param	mem		memory type (Flash or EEPROM)
 * @param	size	size of block
 * @param	address	address inside the page
 */
 void BlockRead(unsigned char mem, unsigned int size, uint16_t * address);

/**
 * Load to memory block
 *
 * @param	mem		memory type (Flash or EEPROM)
 * @param	size	size of block
 * @param	address	address inside the page
 */
 unsigned char BlockWrite(unsigned char mem, unsigned int size, uint16_t *address);

#endif /* _TASK_6_5_H_ */
