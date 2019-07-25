/* INCLUDES ***********************************************************/
#include "ses_rotary.h"

/* DEFINES ************************************************************/
#define ROTARY_PORTA                PORTB
#define ROTARY_PORTB                PORTG
#define ROTARY_PINA						5
#define ROTARY_PINB						2

#define ROTARY_NUM_DEBOUNCE_CHECKS      5

/* GLOBAL VARIABLES ***************************************************/
static pTypeRotaryCallback rotary_clockwiseCallback;
static pTypeRotaryCallback rotary_counterClockwiseCallback;

/* FUNCTIONS **********************************************************/
void rotary_init() {
	//configure the two pins of the rotary to inputs
	DDR_REGISTER(ROTARY_PORTA) &= ~(1<<ROTARY_PINA);
	DDR_REGISTER(ROTARY_PORTB) &= ~(1<<ROTARY_PINB);

	//Enable the internal pull-up resistors
	ROTARY_PORTA |= (1<<ROTARY_PINA);
	ROTARY_PORTB |= (1<<ROTARY_PINB);
}

void rotary_setClockwiseCallback(pTypeRotaryCallback cb) {
	rotary_clockwiseCallback = cb;
}

void rotary_setCounterClockwiseCallback(pTypeRotaryCallback cb) {
	rotary_counterClockwiseCallback = cb;
}

void rotary_checkState() {
    static uint8_t state[ROTARY_NUM_DEBOUNCE_CHECKS] = {};
    static uint8_t index = 0;
    static uint8_t debouncedState = 0;
    uint8_t lastDebouncedState = debouncedState;

    //each bit in every state byte represents one direction of the rotary
    state[index] = 0;

    if(PIN_REGISTER(ROTARY_PORTA) & (1<<ROTARY_PINA)) {
        state[index] |= 1;
    }

    if(PIN_REGISTER(ROTARY_PORTB) & (1<<ROTARY_PINB)) {
        state[index] |= 2;
    }

    index++;

    if (index == ROTARY_NUM_DEBOUNCE_CHECKS) {
    	index = 0;
    }

    /**
      *init compare value and compare with ALL reads, only if
      *we read consistent "1" in the state array, the channel
      *is considered to be ON.
      */
    uint8_t j = 0xFF;

    for(uint8_t i = 0; i < ROTARY_NUM_DEBOUNCE_CHECKS; i++) {
        j = j & state[i];
    }
    debouncedState = j;

    //check if rotary in clockwise
    if ((debouncedState == 3) && (lastDebouncedState == 1)) {
    	rotary_clockwiseCallback();
    }

    //check if rotary in counter-clockwise
    if ((debouncedState == 3) && (lastDebouncedState == 2)) {
    	rotary_counterClockwiseCallback();
    }
}
