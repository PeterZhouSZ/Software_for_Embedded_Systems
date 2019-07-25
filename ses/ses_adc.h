#ifndef SES_ADC_H
#define SES_ADC_H

/*INCLUDES *******************************************************************/
#include <inttypes.h>
#include <avr/io.h>
#include "ses_common.h"

/* DEFINES & MACROS **********************************************************/

// ADC wiring on SES board
#define ADC_PORT       	    PORTF

#define ADC_TEMP_PIN         	        2
#define ADC_LIGHT_PIN         	        4
#define ADC_JOYSTICK_PIN        	    5
#define ADC_MICROPHONE_PIN_1        	0
#define ADC_MICROPHONE_PIN_2        	1

//ADC voltage reference
#define ADC_VREF_SRC                   0x03

//ADC Prescaler
#define ADC_PRESCALE                   0x03

//To signal that the given channel was invalid
#define ADC_INVALID_CHANNEL           0xFFFF

#define ADC_TEMP_MAX 20
#define ADC_TEMP_MIN 40
#define ADC_TEMP_RAW_MAX 480
#define ADC_TEMP_RAW_MIN 256
#define ADC_TEMP_FACTOR 10

enum ADCChannels {
  ADC_MIC_NEG_CH = 0,                   /* ADC0 */
  ADC_MIC_POS_CH ,                      /* ADC1 */
  ADC_TEMP_CH ,                         /* ADC2 */
  ADC_RESERVED1_CH,                     /* ADC3 */
  ADC_LIGHT_CH,                         /* ADC4 */
  ADC_JOYSTICK_CH,                     	/* ADC5 */
  ADC_RESERVED2_CH,                    	/* ADC6 */
  ADC_RESERVED3_CH,                     /* ADC7 */
  ADC_NUM								/* number of ADC channels*/
};

enum JoystickDirections {
  RIGHT = 200,
  UP = 400,
  LEFT = 600,
  DOWN = 800,
  NO_DIRECTION = 1000
};

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes the ADC
 */
void adc_init(void);

/**
 * Read the raw ADC value of the given channel
 *
 * @param	adc_channel	The channel as element of the ADCChannels enum
 *
 * @return	The raw ADC value
 */
uint16_t adc_read(uint8_t adc_channel);

/**
 * Read the current joystick direction
 *
 * @return The direction as element of the JoystickDirections enum
 */
uint8_t adc_getJoystickDirection();

/**
 * Read the current temperature
 *
 * @return Temperature in tenths of degree celsius
 */
int16_t adc_getTemperature();

#endif /* SES_ADC_H */
