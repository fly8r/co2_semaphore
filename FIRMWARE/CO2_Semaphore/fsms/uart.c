/*
 * uart.c
 *
 * Created: 23.01.2019 11:46:11
 *  Author: fly8r
 */
#include "include/uart.h"


/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t		FSM_state;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_UART_Init(void)
{
	// Set default FSM state
	FSM_state = FSM_UART_STATE_IDLE;
}

void FSM_UART_Process(void)
{
	switch(FSM_state) {
		case FSM_UART_STATE_IDLE: {

			return;
		}

		default: break;
	}
}