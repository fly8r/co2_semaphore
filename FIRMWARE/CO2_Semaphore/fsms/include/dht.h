/*
 * dht.h
 *
 * Created: 19.11.2018 15:51:36
 *  Author: fly8r
 */
#ifndef DHT_H_
#define DHT_H_

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"

#include <util/atomic.h>
#include <util/delay.h>

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_DHT_STATES_ENUM
{
	FSM_DHT_STATE_IDLE=0,
	FSM_DHT_STATE_GET_DATA,
	FSM_DHT_STATE_GET_BIT,
	FSM_DHT_STATE_PROCESSING,
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
#define			DHT_REFRESH_PERIOD						1000		// ms
#define			DHT_TIMESLOT_TIMEOUT					100			// us

/************************************************************************/
/* External VARS                                                        */
/************************************************************************/

/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_DHT_Init(void);
// Processing FSM function
void FSM_DHT_Process(void);

#endif /* DHT_H_ */