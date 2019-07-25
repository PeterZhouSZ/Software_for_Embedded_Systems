#ifndef _TASK_6_3_H_
#define _TASK_6_3_H_

/* INCLUDES ***********************************************************/
#include "ses_button.h"
#include "ses_scheduler.h"
#include "ses_lcd.h"
#include "ses_led.h"
#include "ses_uart.h"
#include "ses_rotary.h"

/* TYPES, STRUCTS & ENUMS **********************************************/
/* enum of possible signals */
enum {
	ENTRY,
	EXIT,
	JOYSTICK_PRESSED,
	ROTARY_PRESSED,
	ALARM_CHECK,
	TIMEOUT,
	ROTARY_CLOCKWISE,
	ROTARY_COUNTERCLOCKWISE
};

/* enum of the return values */
enum {
    RET_HANDLED,    //< event was handled
    RET_IGNORED,    //< event was ignored; not used in this implementation
    RET_TRANSITION  //< event was handled and a state transition occurred
};

/* enum to specify the color of the led */
typedef enum {
	RED,
	GREEN,
	YELLOW
} color;

/* typedef for alarm clock state machine */
typedef struct fsm_s Fsm;

/* event type for alarm clock fsm */
typedef struct event_s Event;

/* typedef to be used with return value enum */
typedef uint8_t fsmReturnStatus;

/* for state event handler functions */
typedef fsmReturnStatus (*State)(Fsm *, const Event*);

typedef struct event_s {
    uint8_t signal;			//< identifies the type of event
} Event;

struct fsm_s {
    State state;             //< current state, pointer to event handler
    bool isAlarmEnabled;     //< flag for the alarm status
    struct time_t timeSet;   //< var for the clock
    struct time_t alarmSet;	 //< var for the alarm
};

/* FUNCTION PROTOTYPES ************************************************/
/**
 * Print the clock time in HH:MM format
 *
 * @param	fsm		pointer to the FSM
 */
void lcd_print_updateClkCfg(Fsm* fsm);

/**
 * Print the clock time in HH:MM:SS format
 *
 * @param	param	unused void pointer
 */
void lcd_print_updateClk(void * param);

/**
 * Print the alarm time in HH:MM format
 *
 * @param	fsm		pointer to the FSM
 */
void lcd_print_updateAlarmCfg(Fsm* fsm);

/**
 * send signal when joystick is pressed
 */
static void joystickPressedDispatch(void * param);

/**
 * send signal when the rotary is pressed
 */
static void rotaryPressedDispatch(void * param);

/**
 * send signal when rotary is rotated CW
 */
static void rotaryClockwiseDispatch(void * param);

/**
 * send signal when rotary is rotated CCW
 */
static void rotaryCounterClockwiseDispatch(void * param);

/**
 * send signal when alarm time is matched
 */
static void alarmCheckDispatch(void * param);

/**
 * send signal when alarm times out
 */
static void alarmTimeout(void * pram);

/**
 * dispatches events to state machine
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
inline static void fsm_dispatch(Fsm* fsm, const Event* event);

/**
 * sets and calls initial state of state machine
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
inline static void fsm_init(Fsm* fsm, State init);

/**
 * initial state of the clock
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
fsmReturnStatus clk_init(Fsm* fsm, const Event* e);

/**
 * set hours state of the clock
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
fsmReturnStatus clk_setHour(Fsm* fsm, const Event* e);

/**
 * set minutes state of the clock
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
fsmReturnStatus clk_setMin(Fsm* fsm, const Event* e);

/**
 * running state of the clock
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
fsmReturnStatus clk_run(Fsm* fsm, const Event* e);

/**
 * enabled state of the alarm
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
fsmReturnStatus clk_alarmEnabled(Fsm* fsm, const Event* e);

/**
 * disabled state of the alarm
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
fsmReturnStatus clk_alarmDisabled(Fsm* fsm, const Event* e);

/**
 * state to set the alarm hours
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
fsmReturnStatus clk_setAlarmHour(Fsm* fsm, const Event* e);

/**
 * state to set the alarm minutes
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
fsmReturnStatus clk_setAlarmMin(Fsm* fsm, const Event* e);

/**
 * running state of the alarm
 *
 * @param	fsm		pointer to the FSM
 * @param	e		the event
 */
fsmReturnStatus clk_alarmRun(Fsm* fsm, const Event* e);

/**
 * check if the alarm time is matched or not
 *
 * @param 	fsm		pointer to the FSM
 */
bool check_alarm(Fsm* fsm);

/**
 * debounce the rotary encoder
 *
 * @param	param	unused void pointer
 */
void rotary_debounce(void * param);

/**
 * toggle the LED
 *
 * @param	color	void pointer to led color
 */
void led_toggle(void * color);

#endif /* _TASK_6_3_H_ */
