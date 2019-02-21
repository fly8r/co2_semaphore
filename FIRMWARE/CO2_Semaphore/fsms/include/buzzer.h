/*
 * buzzer.h
 *
 * Created: 19.02.2019 14:49:08
 *  Author: fly8r
 */
#ifndef BUZZER_H_
#define BUZZER_H_

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_BUZZER_STATES_ENUM
{
	FSM_BUZZER_STATE_IDLE=0,
	FSM_BUZZER_STATE_PGM_PROCESSING,
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
#define		BUZZER_PULSE_EQUAL_PERIOD			200	// ms
#define		BUZZER_PULSE_NOT_EQUL_ON_PERIOD		300	// ms
#define		BUZZER_PULSE_NOT_EQUL_OFF_PERIOD	700	// ms

/************************************************************************/
/* External VARS                                                        */
/************************************************************************/



/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_BUZZER_Init(void);
// Processing FSM function
void FSM_BUZZER_Process(void);


#endif