/*
 * lcd_bl_ctrl.h
 *
 * Created: 07.03.2019 10:41:23
 *  Author: fly8r
 */
#ifndef LCDBLCTRL_H_
#define LCDBLCTRL_H_

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"
#include "../../libs/include/tools.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_LCDBLCTRL_STATES_ENUM
{
	FSM_LCDBLCTRL_STATE_IDLE,
	FSM_LCDBLCTRL_STATE_PROCESSING,
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
#define			LCD_BL_CTRL_PERIOD						250

/************************************************************************/
/* External VARS                                                        */
/************************************************************************/

/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_LCDBLCTRL_Init(void);
// Processing FSM function
void FSM_LCDBLCTRL_Process(void);


#endif /* LCDBLCTRL_H_ */