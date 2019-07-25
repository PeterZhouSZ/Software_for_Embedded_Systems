/* INCLUDES ***********************************************************/
#include "task_6_5.h"

/* DEFINES & MACROS ***************************************************/
#define UART_BAUDRATE		  57600
#define PAGESIZE	  		    128		//SPM_PAGESIZE/2
/* BLOCKSIZE should be chosen so that the following holds:
   BLOCKSIZE*n = PAGESIZE,  where n=1,2,3... */
#define BLOCKSIZE 			PAGESIZE
#define APP_END				  0xEFFF	//datasheet page 467
#define DEVTYPE		    	  	0x43	//for ATMega128

/* MAIN ***************************************************************/
int main(void) {
	unsigned char val;
	uint16_t address;
	unsigned int temp_int;

	//Initializations
	uart_init(UART_BAUDRATE);
	led_redInit();
	led_redOff();
	button_init(true);
	sei();

	//function pointer to jump to address 0x0000
	void (* start)(void) = 0x0000;

	//if joystick is pressed, enter the bootloader
	if(button_isJoystickPressed()) {
		//turn red led on
		led_redOn();
		//disable global interrupts
		cli();
		for(;;) {
			val = uart_getc();

			//auto increment address
			if(val == 'a') {
				uart_putc('Y');	//Yes, auto increment is supported

			//set the address
			} else if(val == 'A') {
				address = (uart_getc()<<8) | uart_getc();	//read address high and low
				uart_putc('\r');

			//chip erase
			} else if(val == 'e') {
				for(address = 0; address < APP_END; address += PAGESIZE) {
					boot_spm_busy_wait();	//wait until the page is erased
					boot_page_erase(address);	//erase the page containing the address
				}
				uart_putc('\r');

			//check block load support
			} else if(val == 'b') {
				uart_putc('Y');  //Yes, block load is supported
				uart_putc((BLOCKSIZE >> 8) & 0xFF);	 //MSB first
				uart_putc(BLOCKSIZE & 0xFF);  //Report BLOCKSIZE (bytes)

			//start block load
			} else if(val == 'B') {
				temp_int = (uart_getc()<<8) | uart_getc();	//get the size of the block
				val = uart_getc();	//get memory type
				uart_putc(BlockWrite(val, temp_int, &address));

			//start block read
			} else if(val == 'g') {
				temp_int = (uart_getc()<<8) | uart_getc();	//get the size of the block
				val = uart_getc();	//get memory type
				BlockRead(val, temp_int, &address);

			//enter and leave programming mode.
			} else if((val == 'P') || (val == 'L')) {
				uart_putc('\r');

			//exit the bootloader
			} else if(val == 'E') {
				boot_spm_busy_wait();	//wait until all operations are finished
				boot_rww_enable();	//enable RWW memory
				uart_putc('\r');	//send OK
				start();	//jump to 0x0000

			//return supported device codes
			} else if(val == 't') {
				uart_putc(DEVTYPE);
				uart_putc(0);	//host reads 0

			//set device type
			} else if(val == 'T') {
				//ignore the command and parameters?
				uart_getc();
				uart_putc('\r');

			//return software identifier
			} else if(val == 'S') {
				//software identifier is 7 characters
				//send "SESBOOT"
				uart_putc('S');
				uart_putc('E');
				uart_putc('S');
				uart_putc('B');
				uart_putc('O');
				uart_putc('O');
				uart_putc('T');

			//return software version
			} else if(val == 'V') {
				//version 01
				uart_putc('0');
				uart_putc('1');

			//return signature bytes
			} else if(val == 's') {
				uart_putc(SIGNATURE_2);
				uart_putc(SIGNATURE_1);
				uart_putc(SIGNATURE_0);

			//unsupported command
			} else {
				uart_putc('?');
			}
		}	/* end for(;;) */

	//if not entering the bootloader, execute from 0x0000
	} else {
		boot_spm_busy_wait();	//wait until all operations are finished
		boot_rww_enable();	//enable RWW memory
		start();	//jump to 0x0000
	}
}

/* FUNCTION DEFINITION ************************************************/
unsigned char BlockWrite(unsigned char mem, unsigned int size, uint16_t *address) {
	unsigned char buffer[BLOCKSIZE];	//buffer to temporarily hold the data
	unsigned int data;		//data to be written
	uint16_t tempAddress;

	//EEPROM memory
	if(mem == 'E') {
		/* Fill buffer first, as EEPROM is too slow to copy with UART speed */
		for (tempAddress = 0; tempAddress < size; tempAddress++) {
			buffer[tempAddress] = uart_getc();
		}

		boot_spm_busy_wait();	// wait until all operations are finished

		/* Then program the EEPROM */
		for (tempAddress = 0; tempAddress < size; tempAddress++) {
			EEARL = *address;	//setup EEPROM address
			EEARH = ((*address) >> 8);
			EEDR = buffer[tempAddress];	//get byte
			EECR |= (1 << EEMPE); //write byte
			EECR |= (1 << EEPE);
			while (EECR & (1 << EEPE));	//wait until writing operation is finished

			(*address)++;	//select next EEPROM byte

			return('\r');
		}

	//Flash memory
	} else if(mem == 'F') {
		/* Flash address is not given in words */
		(*address) <<= 1;	//convert address to bytes temporarily
		tempAddress = (*address);	//store address in page
		do {
			data = uart_getc();
			data |= (uart_getc()<<8);
			boot_page_fill(*address,data);
			(*address)+=2;	 //select next word in memory
			size -= 2;  //decrement the number of bytes by 2
		} while(size);
		boot_page_write(tempAddress);
		boot_spm_busy_wait();
		boot_rww_enable();
		(*address) >>= 1;	//convert address back to Flash words again
		return('\r');
	}
	return('?');	//unrecognized command
}

void BlockRead(unsigned char mem, unsigned int size, uint16_t * address) {
	//EEPROM memory
	if(mem == 'E') {
		do {
			EEARL = *address;	//setup EEPROM address
			EEARH = ((*address) >> 8);
			(*address)++;	//select next EEPROM byte
			EECR |= (1 << EERE);	//read EEPROM
			uart_putc(EEDR);	//transmit EEPROM data to PC

			size--;	//decrement number of bytes to read
		} while(size);

	//Flash memory
	} else if(mem == 'F') {
		/* Flash address is not given in words */
		(*address) <<= 1; //convert address to bytes temporarily
		do {
			uart_putc(pgm_read_byte_far(*address));
			uart_putc(pgm_read_byte_far((*address) + 1));
			(*address) += 2;	//select next word in memory
			size -= 2;	//decrement the number of bytes by 2
		} while(size);
		(*address) >>= 1;	//convert address back to Flash words again
	}
}
