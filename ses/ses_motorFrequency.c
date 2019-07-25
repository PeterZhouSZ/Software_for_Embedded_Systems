/* INCLUDES ***********************************************************/
#include "ses_motorFrequency.h"
#include "ses_lcd.h"

/* DEFINES ************************************************************/
#define FREQUENCY_COUNTER_PORT		PORTD
#define FREQUENCY_COUNTER_PIN			0
#define PULSES_PER_REVOLUTION			6
#define TIMER5_FREQUENCY		   250000
#define TIMER5_TOP_250MS			62499
#define NUMBER_OF_READINGS			   27

/* GLOBAL VARIABLES ***************************************************/
volatile static uint16_t pulseCounter = 0;
volatile static uint16_t motorFrequency = 0;
volatile static uint16_t motorFrequencyArr[NUMBER_OF_READINGS];
volatile static uint16_t arrayIndex = 0;

/* FUNCTIONS **********************************************************/
void motorFrequency_init(void) {
	/***** Interrupt 0 Configuration *****/
	//set port as input
	DDR_REGISTER(FREQUENCY_COUNTER_PORT) &= ~(1<<FREQUENCY_COUNTER_PIN);

	//generate IRQ on rising-edge
	EICRA |= (1<<ISC00) | (1<<ISC01);

	//unmask the interrupt
	EIMSK |= (1<<FREQUENCY_COUNTER_PIN);

	//clear the interrupt flag register
	EIFR |= (1<<FREQUENCY_COUNTER_PIN);

	/**** Timer 5 Configuration *****/
	//disable power reduction mode
	PRR1 &= ~(1<<PRTIM5);

	//select CTC Mode
	TCCR5B |= (1<<WGM52);

	//set prescaler to 64
	TCCR5B |= (1<<CS50) | (1<<CS51);

	//set OCR5A to a value for 1 second
	OCR5A = TIMER5_TOP_250MS;

	//unmask the interrupt register
	TIMSK5 |= (1<<OCIE5A);

	//clear the interrupt flag register
	TIFR5 |= (1<<OCF5A);

	//clear the counter for timer 5
	TCNT5 = 0;

	/***** LED Initialization *****/
	led_yellowInit();
	led_greenInit();
	led_yellowOff();
	led_greenOff();

	/***** enable global interrupts *****/
	sei();
}

uint16_t motorFrequency_getRecent(void) {
	return motorFrequency;
}

uint16_t motorFrequency_getMedian(void) {
	uint8_t i, j;
	uint16_t temp;

	//sort the array in ascending order and return the middle value
	for (i = 0; i < NUMBER_OF_READINGS - 1; i++) {
		for (j = i + 1; j < NUMBER_OF_READINGS; j++) {
			if (motorFrequencyArr[j] < motorFrequencyArr[i]) {
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					temp = motorFrequencyArr[i];
					motorFrequencyArr[i] = motorFrequencyArr[j];
					motorFrequencyArr[j] = temp;
				}
			}
		}
	}
	return motorFrequencyArr[((NUMBER_OF_READINGS - 1) / 2)];
}

ISR(INT0_vect) {
	led_yellowToggle();
	led_greenOff();
	pulseCounter++;

	//calculate the frequency after completing one revolution
	if(pulseCounter == PULSES_PER_REVOLUTION) {
		motorFrequency = (uint16_t) (TIMER5_FREQUENCY / (TCNT5 + 1));

		//reset the timer and pulse counter
		TCNT5 = 0;
		pulseCounter = 0;

		//advance the index and store last value in the array
		arrayIndex = (arrayIndex + 1) % NUMBER_OF_READINGS;
		motorFrequencyArr[arrayIndex] = motorFrequency;
	}
}

ISR(TIMER5_COMPA_vect) {
	//toggle the LEDs
	led_greenOn();
	led_yellowOff();

	//set the motor frequency to zero and store the value to the array
	motorFrequency = 0;
	arrayIndex = (arrayIndex + 1) % NUMBER_OF_READINGS;
	motorFrequencyArr[arrayIndex] = motorFrequency;
}
