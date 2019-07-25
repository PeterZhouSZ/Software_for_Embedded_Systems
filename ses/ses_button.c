/* INCLUDES ******************************************************************/
#include "ses_button.h"

/* Global Variables **********************************************************/
pButtonCallback rotaryButtonCallback;
pButtonCallback joystickButtonCallback;

/* FUNCTION DEFINITION *******************************************************/
void button_init(bool debouncing) {
	//Configure buttons pins as inputs
	DDR_REGISTER(BUTTONS_PORT) &= ~((1 << BUTTON_ROTARY_PIN) | (1 << BUTTON_JOYSTICK_PIN));

	// Activating the internal pull-up resistors
	BUTTONS_PORT |= (1 << BUTTON_ROTARY_PIN) | (1 << BUTTON_JOYSTICK_PIN);

	if(debouncing) {
		//Deactivate pin change Interrupt
		PCICR &= ~(1<<PCIE0);

		//initialize timer1
		timer1_start();

	    //setting button debouncing function to the call back function of timer1
		timer1_setCallback(&button_checkState);
	}
	else {
		//Activate pin change Interrupt
		PCICR |= (1<<PCIE0);

		//Pin Change Interrupt enable
		PCMSK0 |= (1<<BUTTON_ROTARY_PIN) | (1<<BUTTON_JOYSTICK_PIN);
	}
}

bool button_isJoystickPressed(void) {
	if(((((PIN_REGISTER(BUTTONS_PORT))>>(BUTTON_JOYSTICK_PIN))&(1u)) == (0u))) {
		return true;
	} else {
		return false;
	}
}

bool button_isRotaryPressed(void) {
	if(((((PIN_REGISTER(BUTTONS_PORT))>>(BUTTON_ROTARY_PIN))&(1u)) == (0u))) {
		return true;
	} else {
		return false;
	}
}


void button_setRotaryButtonCallback(pButtonCallback callback) {
	if (callback != NULL) {
		rotaryButtonCallback = callback;
	}
}

void button_setJoystickButtonCallback(pButtonCallback callback) {
	if (callback != NULL) {
		joystickButtonCallback = callback;
	}
}

void button_checkState() {
    static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {};
    static uint8_t index = 0;
    static uint8_t debouncedState = 0;
    uint8_t lastDebouncedState = debouncedState;

    // each bit in every state byte represents one button
    state[index] = 0;

    if(button_isJoystickPressed()) {
        state[index] |= 1;
    }

    if(button_isRotaryPressed()) {
        state[index] |= 2;
    }

    index++;

    if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
    	index = 0;
    }

    // init compare value and compare with ALL reads, only if
    // we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1" in the state
    // array, the button at this position is considered pressed
    uint8_t j = 0xFF;

    for(uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
        j = j & state[i];
    }
    debouncedState = j;

    //check if joystick button is pressed
    if (((debouncedState & 1) == 1) && (joystickButtonCallback != NULL) && (debouncedState != lastDebouncedState)) {
    	joystickButtonCallback();
    }

    //check if rotary button is pressed
    if ((((debouncedState>>1) & 1) == 1) && (rotaryButtonCallback != NULL) && (debouncedState != lastDebouncedState)) {
        rotaryButtonCallback();
    }
}

ISR(PCINT0_vect) {
	if (button_isJoystickPressed()) {
		if ((GET_BIT(PCMSK0, BUTTON_JOYSTICK_PIN) == (1u)) && (joystickButtonCallback != NULL)) {
			joystickButtonCallback();
		}
	}

	if (button_isRotaryPressed()) {
		if ((GET_BIT(PCMSK0, BUTTON_ROTARY_PIN) == (1u)) && (rotaryButtonCallback != NULL)) {
			rotaryButtonCallback();
		}
	}
}

