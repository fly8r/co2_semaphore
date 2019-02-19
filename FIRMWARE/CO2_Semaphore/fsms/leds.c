/*
 * leds.c
 *
 * Created: 17.02.2019 20:26:36
 *  Author: fly8r
 */
#include "include/leds.h"


/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t				FSM_state;
static					uint8_t				led_bitmap;
static					leds_params_t		*led_params;


/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_LEDS_Init(void)
{
	// Set next FSM state
	FSM_state = FSM_LEDS_STATE_IDLE;
	// Initialize leds bitmap
	led_bitmap=0;
}

void FSM_LEDS_Process(void)
{
	switch(FSM_state) {

		case FSM_LEDS_PGM_STARTUP: {

			if(GetBCMessage(MSG_BC_SYSTEM_STARTUP_COMPLETE)) {
				// Set next FSM state
				FSM_state = FSM_LEDS_STATE_IDLE;
				// Turn OFF RED LED
				LED_RED_OFF();
				// Turn OFF YELLOW LED
				LED_YELLOW_OFF();
				// Turn OFF GREEN LED
				LED_GRN_OFF();
				return;
			}

			if(GetTimer(TIMER_LEDS) >= LEDS_CHANGE_PERIOD) {
				// Change color mechanism
				if(led_bitmap++ > 2) { led_bitmap=1;}

				switch(led_bitmap) {
					// Switch ON RED
					case 1: {
						LED_RED_ON();
						LED_YELLOW_OFF();
						LED_GRN_OFF();
						break;
					}
					// Switch ON YELLOW
					case 2: {
						LED_RED_OFF();
						LED_YELLOW_ON();
						LED_GRN_OFF();
						break;
					}
					// Switch ON GREEN
					case 3: {
						LED_RED_OFF();
						LED_YELLOW_OFF();
						LED_GRN_ON();
						break;
					}
					// Switch OFF all leds
					default: {
						LED_RED_OFF();
						LED_YELLOW_OFF();
						LED_GRN_OFF();
						break;
					}
				}
				// Flush FSM timer
				ResetTimer(TIMER_LEDS);
			}
			return;
		}

		case FSM_LEDS_STATE_IDLE: {

			if(GetBCMessage(MSG_BC_SYSTEM_STARTUP_RUN)) {
				// Set next FSM state
				FSM_state = FSM_LEDS_PGM_STARTUP;
				// Reset FSM timer
				ResetTimer(TIMER_LEDS);
				return;
			}

			if(GetMessage(MSG_LEDS_PROCESSING)) {
				//
				led_params = GetMessageParam(MSG_LEDS_PROCESSING);
				//
				led_params->_active = 1;
				//
				FSM_state = FSM_LEDS_PGM_PROCESSING;
				//
				ResetTimer(TIMER_LEDS);
				//
				ResetTimer(TIMER_LEDS_BLINK);
				return;
			}



			return;
		}

		case FSM_LEDS_PGM_PROCESSING: {

			if(
				(led_params->glow_time_ms != 0xFFFF && GetTimer(TIMER_LEDS) >= led_params->glow_time_ms) ||
				GetMessage(MSG_LEDS_TURN_OFF)
			) {
				//
				LED_RED_OFF();
				LED_YELLOW_OFF();
				LED_GRN_OFF();
				//
				FSM_state = FSM_LEDS_STATE_IDLE;
				return;
			}


			if(led_params->_active) {
				switch(led_params->color) {

					case RED: {
						LED_RED_ON();
						LED_YELLOW_OFF();
						LED_GRN_OFF();
						break;
					}

					case YELLOW: {
						LED_RED_OFF();
						LED_YELLOW_ON();
						LED_GRN_OFF();
						break;
					}

					case GREEN: {
						LED_RED_OFF();
						LED_YELLOW_OFF();
						LED_GRN_ON();
						break;
					}
					default: break;
				}

				if(led_params->_blinking && GetTimer(TIMER_LEDS_BLINK) >= LEDS_TURN_ON_PERIOD) {
					//
					led_params->_active = 0;
					//
					ResetTimer(TIMER_LEDS_BLINK);
				}

			} else {
				//
				LED_RED_OFF();
				LED_YELLOW_OFF();
				LED_GRN_OFF();


				if(led_params->_blinking && GetTimer(TIMER_LEDS_BLINK) >= LEDS_TURN_OFF_PERIOD) {
					//
					led_params->_active = 1;
					//
					ResetTimer(TIMER_LEDS_BLINK);
				}
			}


			return;
		}

		default: break;
	}

}
