#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/
#include "ses_common.h"
#include "ses_timer.h"

/* DEFINES & MACROS **********************************************************/

// Button wiring on SES board
#define BUTTONS_PORT       	    PORTB
#define BUTTON_ROTARY_PIN         	6
#define BUTTON_JOYSTICK_PIN        	7

#define BUTTON_NUM_DEBOUNCE_CHECKS 5

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder and joystick button
 */
void button_init(bool);

/** 
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);

/**
 * Pointer to function
 */
typedef void (*pButtonCallback)();

/**
 * Setter function to store given pointer to function in a variable
 */
void button_setRotaryButtonCallback(pButtonCallback callback);

/**
 * Setter function to store given pointer to function in a variable
 */
void button_setJoystickButtonCallback(pButtonCallback callback);

/**
 * A function to check for button debouncing (press / release)
 */
void button_checkState(void);


#endif /* SES_BUTTON_H_ */
