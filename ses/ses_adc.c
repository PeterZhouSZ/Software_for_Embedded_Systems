/* INCLUDES ******************************************************************/
#include "ses_adc.h"

/* FUNCTION DEFINITION *******************************************************/
void adc_init(void) {
	//Configuring data direction registers of ADC inputs
	CLR_BIT(DDR_REGISTER(ADC_PORT), ADC_TEMP_PIN);
	CLR_BIT(DDR_REGISTER(ADC_PORT), ADC_LIGHT_PIN);
	CLR_BIT(DDR_REGISTER(ADC_PORT), ADC_JOYSTICK_PIN);
	CLR_BIT(DDR_REGISTER(ADC_PORT), ADC_MICROPHONE_PIN_1);
	CLR_BIT(DDR_REGISTER(ADC_PORT), ADC_MICROPHONE_PIN_2);

	//Deactivating the internal pull-up resistors
	CLR_BIT(ADC_PORT, ADC_TEMP_PIN);
	CLR_BIT(ADC_PORT, ADC_LIGHT_PIN);
	CLR_BIT(ADC_PORT, ADC_JOYSTICK_PIN);
	CLR_BIT(ADC_PORT, ADC_MICROPHONE_PIN_1);
	CLR_BIT(ADC_PORT, ADC_MICROPHONE_PIN_2);

	//Disabling power reduction mode of the ADC
	CLR_BIT(PRR0, PRADC);

	//Setting the voltage reference of the ADC
	ADMUX |= (((ADC_VREF_SRC & 0x03) << 6));

	//Making the ADC right adjusted
	CLR_BIT(ADMUX, ADLAR);

	//Adjusting the ADC prescaler F_ADC = F_CPU / Prescaler
	ADCSRA |= (ADC_PRESCALE & 0x07);

	//Disabling the auto trigger
	CLR_BIT(ADCSRA, ADATE);

	//Enabling the ADC
	SET_BIT(ADCSRA,ADEN);
}

uint16_t adc_read(uint8_t adc_channel) {
	uint8_t msb = 0, lsb = 0;
	uint16_t adc = 0;

	//Check if the adc_channel is invalid
	if(adc_channel >= ADC_NUM || adc_channel < 0x00) {
		return ADC_INVALID_CHANNEL;
	} else {
		//Choosing the ADC channel
		ADMUX |= (adc_channel & 0x07);

		//start conversion
		SET_BIT(ADCSRA, ADSC);

		//Busy wait till conversion ends
		while(GET_BIT(ADCSRA, ADSC) == 1u);
		lsb = ADCL;
		msb = ADCH;
		adc = (msb << 8) | lsb;

		return adc;
	}
}

uint8_t adc_getJoystickDirection(void) {
	uint16_t joystick = adc_read(ADC_JOYSTICK_CH);

	if ((joystick <= 300) && (joystick > 100)) {
		return (uint8_t)RIGHT;
	} else if ((joystick <= 500) && (joystick > 300)) {
		return (uint8_t)UP;
	} else if ((joystick <= 700) && (joystick > 500)) {
		return (uint8_t)LEFT;
	} else if ((joystick <= 900) && (joystick > 700)) {
		return (uint8_t)DOWN;
	} else {
		return (uint8_t)NO_DIRECTION;
	}
}

int16_t adc_getTemperature(void) {
	int16_t adc = (int16_t)adc_read(ADC_TEMP_CH);
	int16_t slope = (int16_t)(ADC_TEMP_MAX - ADC_TEMP_MIN) / (ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN);
	int16_t offset = (int16_t)(ADC_TEMP_MAX - (ADC_TEMP_RAW_MAX * slope));
	return (int16_t)(adc * slope + offset) / ADC_TEMP_FACTOR;
}
