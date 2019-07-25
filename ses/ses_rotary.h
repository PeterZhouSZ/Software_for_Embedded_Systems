#ifndef SES_ROTARY_H_
#define SES_ROTARY_H_

/* INCLUDES ***********************************************************/
#include <util/atomic.h>
#include "ses_common.h"
#include "ses_timer.h"
#include "ses_scheduler.h"

/* FUNCTION PROTOTYPES ************************************************/
/**type of function pointer used as rotary encoder call back
 */
typedef void (*pTypeRotaryCallback)();

/*
 * A function to initialize the rotary encoder
 */
void rotary_init();

/*
 * A function to set the call back function of the clockwise
 */
void rotary_setClockwiseCallback(pTypeRotaryCallback);

/*
 * A function to set the call back function of the counter-clockwise
 */
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback);

/*
 * A function to check for the debouncing of the rotary in both directions
 */
void rotary_checkState(void);


#endif /* SES_ROTARY_H_ */
