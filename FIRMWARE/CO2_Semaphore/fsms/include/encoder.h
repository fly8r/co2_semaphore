/*
 * encoder.h
 *
 * Created: 06.12.2018 11:02:52
 *  Author: fly8r
 */
#ifndef ENC_H_
#define ENC_H_

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_ENC_STATES_ENUM
{
	FSM_ENC_STATE_IDLE=0,
	FSM_ENC_STATE_SCAN,
	FSM_ENC_STATE_PROCESSING,
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/

typedef struct {
	int8_t		prev_state,				// Previous pin state
				value,					// Current rotate value with sign
				last_value;				// Last rotate value
} encoder_data_t;


/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_ENC_Init(void);
// Processing FSM function
void FSM_ENC_Process(void);

#endif /* ENC_H_ */
