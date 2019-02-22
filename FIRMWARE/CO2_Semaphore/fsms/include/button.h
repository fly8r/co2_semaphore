/*
 * button.h
 *
 * Created: 31.01.2019 14:24:09
 *  Author: fly8r
 */
#ifndef BUTTON_H_
#define BUTTON_H_

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum BUTTON_STATES_ENUM
{
	BUTTON_STATE_UP,
	BUTTON_STATE_DN,
	BUTTON_STATE_AL,
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
#define			BUTTON_DEBOUNCE_DELAY		10
#define			BUTTON_LONG_PRESS_COUNT		45

typedef struct {
	enum	BUTTON_EVENTS_ENUM		event,
	last_event;
	uint8_t							lp_counter;
} button_data_t;

/************************************************************************/
/* External VARS                                                        */
/************************************************************************/

/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_BUTTON_Init(void);
// Processing FSM function
void FSM_BUTTON_Process(void);
// Return last button event
enum BUTTON_EVENTS_ENUM	FSM_BUTTON_getLastEvent(void);

#endif /* BUTTON_H_ */