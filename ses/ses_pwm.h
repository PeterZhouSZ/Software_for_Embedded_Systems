#ifndef _SES_PWM_H_
#define _SES_PWM_H_

/*INCLUDES *******************************************************************/
#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initialize the PWM functionality
 */
void pwm_init(void);

/**
 * Set the duty cycle of the PWM signal
 *
 * @param dutyCycle	The desired duty cycle of the signal
 */
void pwm_setDutyCycle(uint8_t dutyCycle);

#endif /* _SES_PWM_H_ */
