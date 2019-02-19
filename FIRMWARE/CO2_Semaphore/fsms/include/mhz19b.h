/*
 * mhz19b.h
 *
 * Created: 02.02.2019 13:06:08
 *  Author: fly8r
 */
#ifndef FSM_MHZ19B_H_
#define FSM_MHZ19B_H_

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"
#include "../../drivers/include/uart.h"
#include <avr/pgmspace.h>
#include <util/delay.h>


#include "../../libs/include/tools.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_MHZ19B_STATE_ENUM
{
	FSM_MHZ19B_STATE_IDLE=0,
	FSM_MHZ19B_STATE_STARTUP_INIT,
	FSM_MHZ19B_STATE_STARTUP_TIMEOUT,
	FSM_MHZ19B_STATE_GET_DATA,
	FSM_MHZ19B_STATE_PROCESSING_DATA,
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
#define		MHZ19B_REFRESH_PERIOD			20000		// ms
#define		MHZ19B_REFRESH_TIMEOUT			50			// ms

/************************************************************************/
/* External VARS                                                        */
/************************************************************************/

/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_MHZ19B_Init(void);
// Processing FSM function
void FSM_MHZ19B_Process(void);


#endif /* FSM_MHZ19B_H_ */
