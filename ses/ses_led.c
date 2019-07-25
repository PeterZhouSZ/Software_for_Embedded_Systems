/* INCLUDES ******************************************************************/
#include "ses_led.h"

/* FUNCTION DEFINITION *******************************************************/
void led_redInit(void) {
	DDR_REGISTER(LED_RED_PORT) |= (1 << LED_RED_PIN);
}

void led_redToggle(void) {
	LED_RED_PORT ^= (1 << LED_RED_PIN);
}

void led_redOn(void) {
	LED_RED_PORT &= ~(1 << LED_RED_PIN);
}

void led_redOff(void) {
	LED_RED_PORT |= (1 << LED_RED_PIN);
}

void led_yellowInit(void) {
	DDR_REGISTER(LED_YELLOW_PORT) |= (1 << LED_RED_PIN);
}

void led_yellowToggle(void) {
	LED_YELLOW_PORT ^= (1 << LED_RED_PIN);;
}

void led_yellowOn(void) {
	LED_YELLOW_PORT &= ~(1 << LED_YELLOW_PIN) ;
}

void led_yellowOff(void) {
	LED_YELLOW_PORT |= 1 << LED_YELLOW_PIN;
}

void led_greenInit(void) {
	DDR_REGISTER(LED_GREEN_PORT) |= (1 << LED_RED_PIN);
}

void led_greenToggle(void) {
	LED_GREEN_PORT ^= (1 << LED_RED_PIN);
}

void led_greenOn(void) {
	LED_GREEN_PORT &= (1 << LED_RED_PIN);
}

void led_greenOff(void) {
	LED_GREEN_PORT |= (1 << LED_RED_PIN);
}
