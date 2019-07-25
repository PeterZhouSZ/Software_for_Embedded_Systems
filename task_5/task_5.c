/**********************************************************************
 ***			Software for Embedded Systems - Task 5				***
 *** 						Motor Controller						***
 ***																***
 *** 		 Press rotary button to start or stop the motor			***
 ***																***
 ***		Press joystick button to toggle the PID controller		***
 **********************************************************************/

/* INCLUDES ***********************************************************/
#include "task_5.h"

/* DEFINES & MACROS ***************************************************/
#define MAX(x,y) ((x) >= (y)) ? (x) : (y)
#define MIN(x,y) ((x) <= (y)) ? (x) : (y)

#define HERTZ_TO_RPM		 60

/** constants for LCD plotting area **/
#define PLOT_START_ROW		 16
#define PLOT_END_ROW		 31
#define PLOT_START_COLUMN	  0
#define PLOT_END_COLUMN		121
#define PIXELS_PER_ROW		122

/** limiting values for PWM output **/
#define PWM_MAX				255
#define PWM_MIN				  0

/* GLOBAL VARIABLES ***************************************************/
static taskDescriptor task1;
static taskDescriptor task2;
static taskDescriptor task3;

/* MAIN ***************************************************************/
int main(void) {
	//Initializations
	button_init(false);
	lcd_init();
	pwm_init();
	motorFrequency_init();
	scheduler_init();
	//uart_init(57600);		//used for tuning PID controller

	//set the callback functions for the buttons
	button_setRotaryButtonCallback(&rotary_toggleMotor);
	button_setJoystickButtonCallback(&joystick_togglePid);

	//print the speed and median every second
	task1.period = 1000;
	task1.expire = 1000;
	task1.param = NULL;
	task1.task = &printMotorSpeed;

	//update the PID controller every 10 ms
	uint16_t freq = 100;
	task2.period = 10;
	task2.expire = 10;
	task2.param = (void *) &freq;
	task2.task = &pidController;

	//plot the response of the motor every 10 ms
	task3.period = 10;
	task3.expire = 10;
	task3.param = NULL;
	task3.task = &plotMotorResponse;

	//add the tasks and run the scheduler
	scheduler_add(&task1);
	scheduler_add(&task3);
	scheduler_run();

	return 0;
}

/* FUNCTION DEFINITION ************************************************/
void rotary_toggleMotor(void) {
	//boolean to check if the motor is running or not
	static bool firstRotaryPress = true;
	if(firstRotaryPress) {
		pwm_setDutyCycle(170);
	} else {
		pwm_setDutyCycle(255);
	}
	firstRotaryPress = !firstRotaryPress;
}

void joystick_togglePid(void) {
	//boolean to check if the PID controller is running or not
	static bool pidStatus = false;
	if(!pidStatus) {
		scheduler_add(&task2);
	} else {
		scheduler_remove(&task2);
	}
	pidStatus = !pidStatus;
}

void pidController(void * targetFrequency) {
	uint16_t * setPoint = (uint16_t *) targetFrequency;

	//PID controller gains
	uint8_t Kp = 27;
	uint8_t Ki = 5;
	uint8_t Kd = 3;
	uint8_t Aw = 60;

	//error calculations
	static int16_t errorSum = 0;
	static int16_t errorLast = 0;
	int16_t error = (* setPoint) - motorFrequency_getMedian();
	errorSum += error;
	errorSum = MAX(MIN(errorSum, Aw), -Aw);

	//control signal to the motor (PWM Value)
	int16_t controlOutput = (Kp * error + Ki * errorSum + Kd * (errorLast - error));
	controlOutput = controlOutput / 10;		//divide by a scaling factor

	//constrain the output
	if (controlOutput > PWM_MAX) {
		controlOutput = PWM_MAX;
	} else if (controlOutput < PWM_MIN) {
		controlOutput = PWM_MIN;
	}

	//invert the output (inverted PWM mode)
	controlOutput = 255 - controlOutput;

	//set the duty cycle and update the last error value
	pwm_setDutyCycle(controlOutput);
	errorLast = error;

	//print the values for tuning purpose
	//fprintf(uartout, "Err: %d, errSum: %d, PWM: %d\n", error, errorSum, controlOutput);
}

void printMotorSpeed(void * param) {
	lcd_clear();
	lcd_setCursor(0,0);
	fprintf(lcdout, "Speed = %d RPM", HERTZ_TO_RPM * motorFrequency_getRecent());
	lcd_setCursor(0,1);
	fprintf(lcdout, "Median = %d RPM", HERTZ_TO_RPM * motorFrequency_getMedian());
}

void plotMotorResponse(void * param) {
	//variable to loop over each pixel horizontally
	static uint8_t timeAxis = 0;

	//plot the horizontal axis
	for(int i = PLOT_START_COLUMN; i <= PLOT_END_COLUMN; i++) {
		lcd_setPixel(PLOT_END_ROW, i, 1);
	}

	//plot the vertical axis
	for(int j = PLOT_START_ROW; j <= PLOT_END_ROW; j++) {
		lcd_setPixel(j, PLOT_START_COLUMN, 1);
	}

	//advance the time axis
	timeAxis = (timeAxis + 1) % PIXELS_PER_ROW;

	//scale the values by 10 and return the maximum if it exceeds it
	uint8_t response = MAX(PLOT_END_ROW - (motorFrequency_getMedian() / 10), PLOT_START_ROW);

	//plot the response
	lcd_setPixel(response, timeAxis, 1);
}
