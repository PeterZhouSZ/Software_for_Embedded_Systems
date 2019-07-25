#ifndef SES_MOTORFREQUENCY_H_
#define SES_MOTORFREQUENCY_H_

/* INCLUDES ***********************************************************/
#include <util/atomic.h>
#include "ses_common.h"
#include "ses_led.h"

/* FUNCTION PROTOTYPES ************************************************/

/**
 * Initialize the measurement of motor frequency
 */
void motorFrequency_init(void);

/**
 * Get the most recent measurement of the frequency
 *
 * @return	the most recent reading of motor frequency
 */
uint16_t motorFrequency_getRecent(void);

/**
 * Get the median of the frequency readings
 *
 * @return	the median of the frequency values
 */
uint16_t motorFrequency_getMedian(void);

#endif /* SES_MOTORFREQUENCY_H_ */
