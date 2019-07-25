/* INCLUDES *****************************************************************/
#include "task_4_2.h"

/* GLOBAL VARIABLES *********************************************************/

/*Flag to detect the first press after led is turned off */
static bool firstJoystickPress = true;

/* Tasks to be scheduled */
static taskDescriptor task1;
static taskDescriptor task2;
static taskDescriptor task3;
static taskDescriptor task4;

/* MAIN **********************************************************************/

int main() {
	//Initializations
	lcd_init();
	led_yellowInit();
	led_greenInit();
	button_init(true);

	//Turn off yellow led
	led_yellowOff();

	//Task 1 to toggle green LED at freq = 0.5
	color ledColor = GREEN;
	task1.task = &led_toggle;
	task1.param = (void *) &ledColor;
	task1.expire = 1000;
	task1.period = 1000;

	//Task 2 to debounce buttons every 5 ms
	task2.task = &button_debounce;
	task2.param = NULL;
	task2.expire = 5;
	task2.period = 5;

	//Toggle yellow led when joystick presssed and turns off after 5s
	button_setJoystickButtonCallback(&led_yellowJoystick);
	task3.task = &led_yellowAutoOff;
	task3.param = NULL;
	task3.expire = 5000;
	task3.period = 5000;

	//Start and stops a stop watch by pressing the rotary button
	button_setRotaryButtonCallback(&stopwatch_toggle);
	task4.task = &stopwatch_update;
	task4.param = NULL;
	task4.expire = 100;
	task4.period = 100;

	//add tasks to the scheduler
	scheduler_add(&task1);
    scheduler_add(&task2);

	//initialize the scheduler
	scheduler_init();

	//run the scheduler in a superloop
	scheduler_run();

	return 0;
}

/* FUNCTIONS DEFINITION ******************************************************/

void button_debounce(void * param) {
	button_checkState();
}

void stopwatch_toggle() {
	/* boolean variable to be used for toggling the stopWatch on the LCD */
	static bool stopWatch = true;
	if (stopWatch == true) {
		scheduler_add(&task4);
		stopWatch = false;
		return;
	} else {
		scheduler_remove(&task4);
		stopWatch = true;
	}
}

void stopwatch_update(void * s) {
	static uint16_t tenth = 0x00;
	static uint16_t sec = 0x00;
	tenth++;
	if (tenth >= 10) {
		sec++;
		tenth = 0;
	}
	lcd_clear();
	lcd_setCursor(1,1);
	fprintf(lcdout, "%d:%d", sec, tenth);
}

void led_toggle(void * led) {
	switch(*(color *)led) {
	case RED:
		led_redToggle();
		break;
	case GREEN:
		led_greenToggle();
		break;
	case YELLOW:
		led_yellowToggle();
		break;
	default:
		return;
	}
}

void led_yellowJoystick(void) {
	color ledColor = YELLOW;
	led_toggle((void *) &ledColor);
	if(firstJoystickPress) {
		scheduler_add(&task3);
		firstJoystickPress = false;
	} else {
		scheduler_remove(&task3);
		firstJoystickPress = true;
	}
}

void led_yellowAutoOff(void * param) {
	color ledColor = YELLOW;
	led_toggle((void *) &ledColor);
	firstJoystickPress = true;
	scheduler_remove(&task3);
}
