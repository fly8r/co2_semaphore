/*
 * system.h
 *
 * Created: 23.01.2019 11:31:50
 *  Author: fly8r
 */
#ifndef FSM_SYSTEM_H_
#define FSM_SYSTEM_H_

#include <avr/eeprom.h>
#include <util/atomic.h>
#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"
#include "../../menu.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_SYSTEM_STATES_ENUM
{
	FSM_SYSTEM_STATE_IDLE=0,
	FSM_SYSTEM_STATE_STARTUP,
	FSM_SYSTEM_STATE_STARTUP_COMPLETE,
	FSM_SYSTEM_STATE_CONCENTRATION_PROCESSING,
};



/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
#define			SYSTEM_INITIALIZATION_TIMEOUT			100


/************************************************************************/
/* External VARS                                                        */
/************************************************************************/

/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_SYSTEM_Init(void);
// Processing FSM function
void FSM_SYSTEM_Process(void);



#endif /* FSM_SYSTEM_H_ */
