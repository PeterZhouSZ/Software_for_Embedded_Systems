/* INCLUDES ******************************************************************/
#include <avr/io.h>
#include "ses_led.h"
#include "ses_button.h"
#include "ses_timer.h"

/* MAIN **********************************************************************/
int main(void) {
	//Initialize buttons and LEDs
	led_redInit();
	led_greenInit();
	button_init(true);

	//set the functions to be executed on button press
	button_setJoystickButtonCallback(&led_greenToggle);
	button_setRotaryButtonCallback(&led_redToggle);

	//activate interrupts
	sei();

	while(1) {
		//DO NOTHING
	}
}
