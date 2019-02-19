/*
 * buzzer.c
 *
 * Created: 19.02.2019 14:48:59
 *  Author: fly8r
 */
#include "include/buzzer.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/

void FSM_BUZZER_Init(void)
{
	FSM_state = FSM_BUZZER_STATE_IDLE;
}

void FSM_BUZZER_Process(void)
{
	switch(FSM_state) {


		default: break;
	}

}
