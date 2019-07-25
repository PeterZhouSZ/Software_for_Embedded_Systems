/* INCLUDES ******************************************************************/
#include "ses_timer.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC		9999
#define TIMER2_CYC_FOR_1MILLISEC		 249

/* GLOBAL VARIABLES **********************************************************/
pTimerCallback timer2Callback;
pTimerCallback timer1Callback;

/*FUNCTION DEFINITION ********************************************************/
void timer2_setCallback(pTimerCallback cb) {
	if (cb != NULL) {
		timer2Callback = cb;
	}
}

void timer2_start() {
	//Set Timer2 to CTC mode
	TCCR2A |= 0x02;

	//Set the value for output compare A
	OCR2A = TIMER2_CYC_FOR_1MILLISEC;

	//Set the precaler of Timer2 to 64
	TCCR2B |= 0x04;

	//Set timer2 interrupt to compare A
	TIMSK2 |= (1 << OCIE2A);

	//Clear the timer2 flag of compare output A
	TIFR2 |= (1 << OCF2A);

	//Clear the counter of Timer 2
	TCNT2 = 0;
}

void timer2_stop() {
	//by setting the prescaler to no clock source
	CLR_BIT(TCCR2B, CS20);
	CLR_BIT(TCCR2B, CS21);
}

void timer1_setCallback(pTimerCallback cb) {
	if (cb != NULL) {
		timer1Callback = cb;
	}
}

void timer1_start() {
	//Set Timer1 to CTC mode
	TCCR1A |= 0x04;

	//Set the value for output compare A
	OCR1A = TIMER1_CYC_FOR_5MILLISEC;

	//Set the precaler of Timer1 to 8
	TCCR1B |= 0x02;

	//Set timer1 interrupt to compare A
	TIMSK1 |= (1 << OCIE1A);

	//Clear the timer1 flag of compare output A
	TIFR1 |= (1 << OCF1A);

	//Clear the counter of Timer 1
	TCNT1 = 0;
}

void timer1_stop() {
	//by setting the prescaler to no clock source
	CLR_BIT(TCCR1B, CS20);
	CLR_BIT(TCCR1B, CS21);
}

ISR(TIMER1_COMPA_vect) {
	if (timer1Callback != NULL) {
		timer1Callback();
	}
}

ISR(TIMER2_COMPA_vect) {
	if (timer2Callback != NULL) {
		timer2Callback();
	}
}
