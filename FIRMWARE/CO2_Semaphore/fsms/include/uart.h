/*
 * uart.h
 *
 * Created: 23.01.2019 11:44:44
 *  Author: fly8r
 */
#ifndef FSM_UART_H_
#define FSM_UART_H_

#include "../../global_vars.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"
#include "../../drivers/include/uart.h"


/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_UART_STATES_ENUM
{
	FSM_UART_STATE_IDLE=0,
	FSM_UART_STATE_BUFF_PROCESSING,
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
void FSM_UART_Init(void);
// Processing FSM function
void FSM_UART_Process(void);


#endif /* FSM_UART_H_ */
