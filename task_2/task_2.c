/* INCLUDES ******************************************************************/
#include "ses_lcd.h"
#include "ses_uart.h"
#include "ses_led.h"
#include "ses_button.h"
#include "ses_adc.h"
#include <avr/io.h>
#include <util/delay.h>

/* MAIN **********************************************************************/
int main(void) {
	//Initialize Peripherals
	button_init(false);
	adc_init();
	lcd_init();
	led_redInit();
	led_greenInit();
	led_yellowInit();

	//set UART baudrate
	uart_init(57600);

	//Test LCD & UART
	fprintf(uartout, "START\n");
	fprintf(lcdout, "START");

	while(1) {
		led_redOff();
		led_greenOff();
		led_yellowOff();

		while(button_isRotaryPressed()) {
			led_redOn();
		}
		while(button_isJoystickPressed()) {
			led_greenOn();
		}
		while(adc_getJoystickDirection() == LEFT) {
			led_yellowOn();
		}

		lcd_clear();
		int16_t temp = adc_getTemperature();
		fprintf(lcdout, "Temprature: %d\n", temp);
		uint16_t light = adc_read(ADC_LIGHT_CH);
		lcd_setCursor(2, 1);
		fprintf(lcdout, "Light Sensor: %d\n", light);
		_delay_ms(2500);
	}
}
