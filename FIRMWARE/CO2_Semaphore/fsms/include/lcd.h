/*
 * lcd.h
 *
 * Created: 23.01.2019 11:49:28
 *  Author: fly8r
 */
#ifndef FSM_LCD_H_
#define FSM_LCD_H_

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"
#include "../../libs/include/hd44780_cc.h"
#include "../../libs/include/tools.h"
#include "../../fsms/include/pcf8574.h"
#include "../../fsms/include/mhz19b.h"
#include "../../menu.h"
#include "../../strings.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_LCD_STATES_ENUM
{
	FSM_LCD_STATE_IDLE=0,
	FSM_LCD_STATE_BL_CTRL,
	FSM_LCD_STATE_LCD_INIT,
	FSM_LCD_STATE_SPLASH_DRAW,
	FSM_LCD_STATE_SPLASH_SHOW,
	FSM_LCD_STATE_SPLASH_HIDE_ANIMATION,
	FSM_LCD_STATE_DRAW_SENSORS_STATE_SCREEN,
	FSM_LCD_STATE_ANIMATION_SENSORS_STATE_SCREEN,
	FSM_LCD_STATE_REFRESH_SCREEN,
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
//> Custom char size: 0 - Unsupported, 2 - 2 rows chars, 3 - 3 rows chars
#define			LCD_SUPPORT_CUSTOM_CHARS_SIZE	3
//>
#define			LCD_REFRESH_PERIOD						500
#define			LCD_BL_CTRL_PERIOD						1000
#define			LCD_SPLASH_SLIDE_PERIOD					25
#define			LCD_SPLASH_SPLASH_HIDE_TIMEOUT			2000

/************************************************************************/
/* External VARS                                                        */
/************************************************************************/

/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_LCD_Init(void);
// Processing FSM function
void FSM_LCD_Process(void);


#endif /* FSM_LCD_H_ */
