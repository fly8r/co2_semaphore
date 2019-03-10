/*
 * charts.c
 *
 * Created: 10.03.2019 21:32:04
 *  Author: fly8r
 */ 
#include "include/charts.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_CHARTS_Init(void)
{
	FSM_state = FSM_CHARTS_STATE_IDLE;
}

void FSM_CHARTS_Process(void)
{
	switch(FSM_state) {
		
		case FSM_CHARTS_STATE_IDLE: {
			
			break;
		}
		
		default: break;
	}
	
}