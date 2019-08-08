#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>;

uint8_t revBits(uint8_t num);

int main(void) {
	uint8_t var = 0x00;

	//Setting bit 3
	var |= (1<<3);
	printf("The result of setting bit 3 is 0x%02x\n", var);

	//Set bits 4 and 6 with bit shifting
	var |= (1<<4) | (1<<6);
	printf("The result of setting bit 4 and 6 is 0x%02x\n", var);

	//Setting bit 4 and 6 without bit shifting
	var |= 0x50;
	printf("The result of setting bit 4 and 6 is 0x%02x\n", var);

	//clearing bit 2
	var &= ~(1<<2);
	printf("The result of clearing bit 2 is 0x%02x\n", var);

	//Toggling bit 3
	var ^= (1<<3);
	printf("The result of toggling bit 3 is 0x%02x\n", var);

	//Setting bit 2 and clear 5 and 7 at the same time
	var |= (1<<2) & ~(1<<5) & ~(1<<7);
	printf("The result of setting bit 2 and clear 5 and 7 at the same time is 0x%02x\n", var);

	//Swap bits 0 - 2 with 3 - 5;
	var = (var & 0b11000000) | ((var & 0b00000111) << 3) | ((var & 0b00111000) >> 3);
	printf("The result of swapping 0 - 2 with 3 - 5 is 0x%02x\n", var);

	//Reversing the order of bits in var
	var = revBits(var);
	printf("The result of reversing the order of bits is 0x%02x\n", var);

	return 0;
}

uint8_t revBits(uint8_t num) {
	uint8_t temp;
	uint8_t revNum = 0;
	uint8_t NUM_BITS = 8;
	int i;
	//check if the ith bit is set. If so, set the (7 - i)th bit
	for(i=0; i<NUM_BITS; i++) {
		temp = num & (1<<i);
		if(temp) {
			revNum |= 1 << ((NUM_BITS - 1) - i);
		}
	}
	return revNum;
}
