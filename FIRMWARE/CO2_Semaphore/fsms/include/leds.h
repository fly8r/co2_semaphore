/*
 * leds.h
 *
 * Created: 17.02.2019 20:26:45
 *  Author: fly8r
 */
#ifndef LEDS_H_
#define LEDS_H_

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_LEDS_STATES_ENUM
{
	FSM_LEDS_STATE_IDLE=0,
	FSM_LEDS_PGM_STARTUP,
	FSM_LEDS_PGM_PROCESSING
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
#define		LEDS_CHANGE_PERIOD				500
#define		LEDS_TURN_ON_PERIOD				500
#define		LEDS_TURN_OFF_PERIOD			500

/************************************************************************/
/* External VARS                                                        */
/************************************************************************/



/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_LEDS_Init(void);
// Processing FSM function
void FSM_LEDS_Process(void);



#endif /* LEDS_H_ */