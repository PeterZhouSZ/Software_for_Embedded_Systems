/* INCLUDES ***********************************************************/
#include "task_6_3.h"

/* DEFINES & MACROS ***************************************************/
#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)

#define HOUR_TO_MILLI	3600000
#define SEC_TO_MILLI	  60000

/* GLOBAL VARIABLES ***************************************************/
static Fsm clkFsm;

/* tasks for the scheduler */
static taskDescriptor task1; //toggle Green led with the clock seconds
static taskDescriptor task2; //checks if the alarm time = current time
static taskDescriptor task3; //show current time
static taskDescriptor task4; //turn off the alarm after 5 seconds
static taskDescriptor task5; //toggle the red LED at 4 Hz
static taskDescriptor task6; //Rotary debouncing

/* MAIN ***************************************************************/
int main(void) {
	//set the callback functions for the buttons
	button_setJoystickButtonCallback(&joystickPressedDispatch);
	button_setRotaryButtonCallback(&rotaryPressedDispatch);

	//set the callback functions for the rotary directions
	rotary_setClockwiseCallback(&rotaryClockwiseDispatch);
	rotary_setCounterClockwiseCallback(&rotaryCounterClockwiseDispatch);

	//Toggle Green LED every 1 second
	color ledGreen = GREEN;
	task1.task = &led_toggle;
	task1.param = (void *) &ledGreen;
	task1.expire = 1000;
	task1.period = 1000;

	//Check the alarm with time every 1 second
	task2.task = &alarmCheckDispatch;
	task2.param = NULL;
	task2.expire = 1000;
    task2.period = 1000;

    //Update the display with the current time
    task3.task = &lcd_print_updateClk;
    task3.param = NULL;
    task3.expire = 1000;
    task3.period = 1000;

	//Turn off the alarm after 5 seconds
    task4.task = &alarmTimeout;
    task4.param = NULL;
    task4.expire = 5000;
    task4.period = 0;

    //Toggle the red LED at 4 Hz
    color ledRed = RED;
    task5.task = &led_toggle;
    task5.param = (void *) &ledRed;
    task5.expire = 250;
    task5.period = 250;

	//Rotary encoder debouncing
	task6.task = &rotary_debounce;
	task6.param = NULL;
	task6.expire = 5;
	task6.period = 5;

	//initialization
	lcd_init();
	rotary_init();
	button_init(true);
	led_redInit();
	led_greenInit();
	led_yellowInit();
	scheduler_init();
	fsm_init(&clkFsm, clk_init);
	sei();

	//turn off the LEDs
	led_redOff();
	led_greenOff();
	led_yellowOff();

	//add tasks and run the scheduler
	scheduler_add(&task6);
	scheduler_run();

	return(0);
}

/* FUNCTION DEFINITION ************************************************/
static void joystickPressedDispatch(void * param) {
    Event e = {.signal = JOYSTICK_PRESSED};
    fsm_dispatch(&clkFsm, &e);
}

static void rotaryPressedDispatch(void * param) {
    Event e = {.signal = ROTARY_PRESSED};
    fsm_dispatch(&clkFsm, &e);
}

static void rotaryClockwiseDispatch(void * param) {
    Event e = {.signal = ROTARY_CLOCKWISE};
    fsm_dispatch(&clkFsm, &e);
}

static void rotaryCounterClockwiseDispatch(void * param) {
    Event e = {.signal = ROTARY_COUNTERCLOCKWISE};
    fsm_dispatch(&clkFsm, &e);
}

static void alarmCheckDispatch(void * param) {
    Event e = {.signal = ALARM_CHECK};
    fsm_dispatch(&clkFsm, &e);
}

static void alarmTimeout(void * pram) {
	Event e = {.signal = TIMEOUT};
	fsm_dispatch(&clkFsm, &e);
}

inline static void fsm_dispatch(Fsm* fsm, const Event* event) {
    static Event entryEvent = {.signal = ENTRY};
    static Event exitEvent = {.signal = EXIT};
    State s = fsm->state;
    fsmReturnStatus r = fsm->state(fsm, event);
    if (r==RET_TRANSITION) {
        s(fsm, &exitEvent);           //< call exit action of last state
        fsm->state(fsm, &entryEvent); //< call entry action of new state
    }
}

inline static void fsm_init(Fsm* fsm, State init) {
    Event entryEvent = {.signal = ENTRY};
    fsm->state = init;
    fsm->state(fsm, &entryEvent);
}

fsmReturnStatus clk_init(Fsm* fsm, const Event* e) {
	switch(e->signal) {
	    case ENTRY:
	    	//Reset the clock and alarm time and print set time on display
	    	fsm->isAlarmEnabled = false;
	    	fsm->timeSet.hour = 0;
	    	fsm->timeSet.minute = 0;
	    	fsm->timeSet.second = 0;
	    	fsm->timeSet.milli = 0;
	    	lcd_print_updateClkCfg(fsm);
	    	return RET_HANDLED;

		case ROTARY_PRESSED:
			//increment the time and switch to state to set the hour
			fsm->timeSet.hour++;
			return TRANSITION(clk_setHour);

		case ROTARY_CLOCKWISE:
			//increment the time and switch to state to set the hour
			fsm->timeSet.hour++;
			return TRANSITION(clk_setHour);

		case ROTARY_COUNTERCLOCKWISE:
			//decrement the time and switch to set the hour
			fsm->timeSet.hour = 23;
			return TRANSITION(clk_setHour);

		case JOYSTICK_PRESSED:
			//move to the next state to set the minutes
			return TRANSITION(clk_setMin);

		default:
			return RET_IGNORED;
	}
}

fsmReturnStatus clk_setHour(Fsm* fsm, const Event* e) {
	switch(e->signal) {
		case ENTRY:
			//update the configuration of the clock
			lcd_print_updateClkCfg(fsm);
			return RET_HANDLED;

		case ROTARY_PRESSED:
			//increment the clock hours and update the display
			fsm->timeSet.hour++;
			if (fsm->timeSet.hour >= 24) {
				fsm->timeSet.hour = 0;
			}
			lcd_print_updateClkCfg(fsm);
			return RET_HANDLED;

		case ROTARY_CLOCKWISE:
			//increment the clock hours and update the display
			fsm->timeSet.hour++;
			if (fsm->timeSet.hour >= 24) {
				fsm->timeSet.hour = 0;
			}
			lcd_print_updateClkCfg(fsm);
			return RET_HANDLED;

		case ROTARY_COUNTERCLOCKWISE:
			//decrement the clock hours and update the display
			fsm->timeSet.hour--;
			if (fsm->timeSet.hour >= 24) {
				fsm->timeSet.hour = 23;
			}
			lcd_print_updateClkCfg(fsm);
			return RET_HANDLED;

		case JOYSTICK_PRESSED:
			//move to the next state to set the clock minutes
			return TRANSITION(clk_setMin);

		default:
			return RET_IGNORED;
	}
}

fsmReturnStatus clk_setMin(Fsm* fsm, const Event* e) {
	switch(e->signal) {
		case ENTRY:
			//update the configuration of the clock
			lcd_print_updateClkCfg(fsm);
			return RET_HANDLED;

		case EXIT:
			//set the scheduler time to the time given by the user
			scheduler_setTime((fsm->timeSet.hour * HOUR_TO_MILLI)
					+ (fsm->timeSet.minute * SEC_TO_MILLI));
			return RET_HANDLED;

		case ROTARY_PRESSED:
			//increment the minutes and update the display
			fsm->timeSet.minute++;
			if (fsm->timeSet.minute >= 60) {
				fsm->timeSet.minute = 0;
			}
			lcd_print_updateClkCfg(fsm);
			return RET_HANDLED;

		case ROTARY_CLOCKWISE:
			//increment the minutes and update the display
			fsm->timeSet.minute++;
			if (fsm->timeSet.minute >= 60) {
				fsm->timeSet.minute = 0;
			}
			lcd_print_updateClkCfg(fsm);
			return RET_HANDLED;

		case ROTARY_COUNTERCLOCKWISE:
			//decrement the minutes and update the display
			fsm->timeSet.minute--;
			if (fsm->timeSet.minute >= 60) {
				fsm->timeSet.minute = 59;
			}
			lcd_print_updateClkCfg(fsm);
			return RET_HANDLED;

		case JOYSTICK_PRESSED:
			//switch to the state to run the clock
			return TRANSITION(clk_run);

		default:
			return RET_IGNORED;
	}
}

fsmReturnStatus clk_run(Fsm* fsm, const Event* e) {
	static bool initial_entry = true;
	switch(e->signal) {
		case ENTRY:
			if(initial_entry) {
				//blink the green led
				scheduler_add(&task1);
				initial_entry = false;
			}
			//display time on the lcd
			scheduler_add(&task3);
			return RET_HANDLED;

		case ROTARY_PRESSED:
			//enable or disable the alarm based on its state
			if(!(fsm->isAlarmEnabled)) {
				led_yellowOn();
				scheduler_add(&task2);
				fsm->isAlarmEnabled = true;
			} else {
				scheduler_remove(&task2);
				led_yellowOff();
				fsm->isAlarmEnabled = false;
			}
			return RET_HANDLED;

		case JOYSTICK_PRESSED:
			//disable the alarm and switch to alarm set state
			scheduler_remove(&task3);
			if(fsm->isAlarmEnabled) {
				scheduler_remove(&task2);
				led_yellowOff();
				fsm->isAlarmEnabled = false;
			}
			return TRANSITION(clk_setAlarmHour);

		case ALARM_CHECK:
			//check if alarm is enabled and alarm = actual time to ring the alarm
			if (check_alarm(fsm) && fsm->isAlarmEnabled) {
				return TRANSITION(clk_alarmRun);
			} else {
				return RET_IGNORED;
			}

		default:
			return RET_IGNORED;
	}
}

fsmReturnStatus clk_setAlarmHour(Fsm* fsm, const Event* e) {
	switch(e->signal) {
		case ENTRY:
			//reset the struct of timeSet
	    	fsm->alarmSet.hour = 0;
	    	fsm->alarmSet.minute = 0;
	    	fsm->alarmSet.second = 0;
	    	fsm->alarmSet.milli = 0;

			//update the lcd with that of the alarm
	    	scheduler_remove(&task3);
			lcd_print_updateAlarmCfg(fsm);
			return RET_HANDLED;

		case ROTARY_PRESSED:
			//increment the alarm hours and update the display
			fsm->alarmSet.hour++;
			if (fsm->alarmSet.hour >= 24) {
				fsm->alarmSet.hour = 0;
			}
			lcd_print_updateAlarmCfg(fsm);
			return RET_HANDLED;

		case ROTARY_CLOCKWISE:
			//increment the alarm hours and update the display
			fsm->alarmSet.hour++;
			if (fsm->alarmSet.hour >= 24) {
				fsm->alarmSet.hour = 0;
			}
			lcd_print_updateAlarmCfg(fsm);
			return RET_HANDLED;

		case ROTARY_COUNTERCLOCKWISE:
			//decrement the alarm hours and update the display
			fsm->alarmSet.hour--;
			if (fsm->alarmSet.hour >= 24) {
				fsm->alarmSet.hour = 23;
			}
			lcd_print_updateAlarmCfg(fsm);
			return RET_HANDLED;

		case JOYSTICK_PRESSED:
			//move to the next state to set the clock minutes
			return TRANSITION(clk_setAlarmMin);

		default:
			return RET_IGNORED;
	}
}

fsmReturnStatus clk_setAlarmMin(Fsm* fsm, const Event* e) {
	switch (e->signal) {
		case ROTARY_PRESSED:
			//increment the alarm minutes and update the display
			fsm->alarmSet.minute++;
			if (fsm->alarmSet.minute >= 60) {
				fsm->alarmSet.minute = 0;
			}
			lcd_print_updateAlarmCfg(fsm);
			return RET_HANDLED;

		case ROTARY_CLOCKWISE:
			//increment the alarm minutes and update the display
			fsm->alarmSet.minute++;
			if (fsm->alarmSet.minute >= 60) {
				fsm->alarmSet.minute = 0;
			}
			lcd_print_updateAlarmCfg(fsm);
			return RET_HANDLED;

		case ROTARY_COUNTERCLOCKWISE:
			//decrement the alarm minutes and update the display
			fsm->alarmSet.minute--;
			if (fsm->alarmSet.minute >= 60) {
				fsm->alarmSet.minute = 59;
			}
			lcd_print_updateAlarmCfg(fsm);
			return RET_HANDLED;

		case JOYSTICK_PRESSED:
			//move to the next state to run the clock
			return TRANSITION(clk_run);

		default:
			return RET_IGNORED;
	}
}

fsmReturnStatus clk_alarmRun(Fsm* fsm, const Event* e) {
	switch (e->signal) {
		case ENTRY:
			//toggle the red led and set alarm to turn off after 5 sec
			task4.expire = 5000;
			scheduler_add(&task4);
			scheduler_add(&task5);
			return RET_HANDLED;

		case EXIT:
			//stop toggling the led and disable the alarm
			scheduler_remove(&task3);
			scheduler_remove(&task5);
			led_redOff();
			led_yellowOff();
			fsm->isAlarmEnabled = false;
			return RET_HANDLED;

		case TIMEOUT:
			//go to the clock run state
			return TRANSITION(clk_run);

		case JOYSTICK_PRESSED:
			//disable auto off & go to the clock run state
			scheduler_remove(&task4);
			return TRANSITION(clk_run);

		case ROTARY_PRESSED:
			//disable auto off & go to the clock run state
			scheduler_remove(&task4);
			return TRANSITION(clk_run);

		default:
			return RET_IGNORED;
	}
}

bool check_alarm(Fsm* fsm) {
	//Ask for the actual time from the scheduler and compare it with the alarm time
	struct time_t actualTime = scheduler_getTimeFormated();
	if(actualTime.hour == fsm->alarmSet.hour && actualTime.minute == fsm->alarmSet.minute) {
		return true;
	}
	return false;
}

void lcd_print_updateClkCfg(Fsm * fsm){
	//clear the screen and print the time to be set on the first line
	lcd_clear();
	lcd_setCursor(7,0);
	fprintf(lcdout, "%02d:%02d", fsm->timeSet.hour, fsm->timeSet.minute);

	//print a message to the user on the following lines
	lcd_setCursor(0,1);
	fprintf(lcdout, "Please set the time");

	lcd_setCursor(0,3);
	fprintf(lcdout, "JS:Ok      RB:Adjust");
}

void lcd_print_updateClk(void * param){
	//clear the screen and print the actual time on the first line
	struct time_t actualTime = scheduler_getTimeFormated();
	lcd_clear();
	lcd_setCursor(6,0);
	fprintf(lcdout, "%02d:%02d:%02d", actualTime.hour, actualTime.minute, actualTime.second);

	//print a message to the user on the following lines
	lcd_setCursor(3,2);
	fprintf(lcdout, "Alarm Controls");

	lcd_setCursor(0,3);
	fprintf(lcdout, "JS:Set         RB:On");
}

void lcd_print_updateAlarmCfg(Fsm* fsm){
	//clear the screen and print the clock time to be set on the first line
	lcd_clear();
	lcd_setCursor(7,0);
	fprintf(lcdout, "%02d:%02d", fsm->alarmSet.hour, fsm->alarmSet.minute);

	//print a message to the user on the following lines
	lcd_setCursor(0,1);
	fprintf(lcdout, "Please set the alarm");

	lcd_setCursor(0,3);
	fprintf(lcdout, "JS:Ok      RB:Adjust");
}

void rotary_debounce(void * param) {
	rotary_checkState();
}

void led_toggle(void * led) {
	switch(*(color *) led) {
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
		break;
	}
}
