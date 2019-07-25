/* INCLUDES ***********************************************************/
#include "ses_pwm.h"

/* DEFINES ************************************************************/
#define MOTOR_PORT	PORTG
#define MOTOR_PIN		5

/* FUNCTION DEFINITION ************************************************/

void pwm_init(void) {
	//set pin as output
	DDR_REGISTER(MOTOR_PORT) |= (1<<MOTOR_PIN);

	//disable power reduction mode to enable Timer 0
	PRR0 &= ~(1<<PRTIM0);

	//select fast PWM mode
	TCCR0A |= (1<<WGM00) | (1<<WGM01);
	TCCR0B |= (1<<WGM02);

	//force on compare output
	TCCR0B &= ~(1<<FOC0B) & ~(1<<FOC0A);

	//set OC0B on Compare Match
	TCCR0A |= (1<<COM0B0) | (1<<COM0B1);

	//set initial duty cycle to zero
	OCR0A = 0xFF;
	OCR0B = 0xFF;

	//disable prescaler
	TCCR0B |= (1<<CS00);
}

void pwm_setDutyCycle(uint8_t dutyCycle) {
	OCR0B = dutyCycle;
}
