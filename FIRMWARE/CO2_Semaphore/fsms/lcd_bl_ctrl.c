/*
 * lcd_bl_ctrl.c
 *
 * Created: 07.03.2019 10:41:12
 *  Author: fly8r
 */
#include "include/lcd_bl_ctrl.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/

void FSM_LCDBLCTRL_Init(void)
{
	FSM_state = FSM_LCDBLCTRL_STATE_IDLE;
	ResetTimer(TIMER_LCD_BL);
}

void FSM_LCDBLCTRL_Process(void)
{
	switch(FSM_state) {

		case FSM_LCDBLCTRL_STATE_IDLE: {

			if(device.flags._setup) {
				if(GetMessage(MSG_LCD_BL_CTRL)) {
					uint8_t *t = GetMessageParam(MSG_LCD_BL_CTRL);
					BL_CTRL_OCR = pgm_read_byte(bl_pwm_table + *t);
				}
			} else {
				if(GetTimer(TIMER_LCD_BL) >= LCD_BL_CTRL_PERIOD) {
					FSM_state = FSM_LCDBLCTRL_STATE_PROCESSING;
					ResetTimer(TIMER_LCD_BL);
				}
			}

			return;
		}

		case FSM_LCDBLCTRL_STATE_PROCESSING: {

			uint16_t curr_time = (rtc.hour << 8) + rtc.min;
			uint16_t start_time = (device.settings.lcd.from_hour << 8) + device.settings.lcd.from_min;
			uint16_t end_time = (device.settings.lcd.to_hour << 8) + device.settings.lcd.to_min;

			if(timeinrange(curr_time, start_time, end_time)) {
				BL_CTRL_OCR = pgm_read_byte(bl_pwm_table + device.settings.lcd.bl_pwm_by_time);
			} else {
				BL_CTRL_OCR = pgm_read_byte(bl_pwm_table + device.settings.lcd.bl_pwm_default);
			}

			// Return to default state
			FSM_state = FSM_LCDBLCTRL_STATE_IDLE;
			return;
		}


		default: break;
	}
}