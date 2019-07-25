/* INCLUDES ******************************************************************/
#include <avr/io.h>

/* PROTOTYPES ****************************************************************/
void wait(uint16_t millis);

/* MAIN **********************************************************************/
int main(void) {
	DDRG |= 0x02;
	while (1) {
		wait(1000);
		PORTG ^= 0x02;
	}
	return 0;
}

/* FUNCTIONS DEFINITION ******************************************************/
void wait(uint16_t millis) {
	uint16_t i, j;
	for(j = millis; j > 0; j--) {
		//delay for 1 millisecond
		for(i = 0x0379; i > 0; i--) {
			asm volatile ("nop");
		}
	}
}
