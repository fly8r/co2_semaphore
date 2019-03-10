/*
 * charts.h
 *
 * Created: 10.03.2019 21:32:13
 *  Author: fly8r
 */ 
#ifndef CHARTS_H_
#define CHARTS_H_

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_CHARTS_STATES_ENUM
{
	FSM_CHARTS_STATE_IDLE=0,
	
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/

/************************************************************************/
/* External VARS                                                        */
/************************************************************************/


/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_CHARTS_Init(void);
// Processing FSM function
void FSM_CHARTS_Process(void);


#endif /* CHARTS_H_ */
