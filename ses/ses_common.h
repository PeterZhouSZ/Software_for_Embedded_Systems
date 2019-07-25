#ifndef SES_COMMON_H_
#define SES_COMMON_H_

/* INCLUDES ******************************************************************/

#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* DEFINES & MACROS **********************************************************/
/**pin address of port x
 *
 * example: PING == PIN_REGISTER(PORTG)
 */
#define PIN_REGISTER(x) (*(&(x) - 2))

/**address of data direction register of port x
 *
 * example: DDRG == DDR_REGISTER(PORTG)
 */
#define DDR_REGISTER(x) (*(&(x) - 1))

//Bitwise operations
#define CLR_BIT(Var,BitNum) ((Var)&=~(1u<<(BitNum)))
#define SET_BIT(Var,BitNum) ((Var)|= (1u<<(BitNum)))
#define TGL_BIT(Var,BitNum) ((Var)^= (1u<<(BitNum)))
#define GET_BIT(Var,BitNum) (((Var)>>(BitNum))&(1u))


enum {
	SES_SUCCESS = 0,
	SES_INVALID = 1,
	SES_FAIL = 2
};
typedef uint8_t ses_error_t;

#endif /* SES_COMMON_H_ */
