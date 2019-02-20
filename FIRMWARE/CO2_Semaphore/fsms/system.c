/*
 * system.c
 *
 * Created: 23.01.2019 11:32:01
 *  Author: fly8r
 */
#include	"include/system.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;
static					leds_params_t	led_params;


/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_SYSTEM_Init(void)
{
	// Set default concentration level to unknown
	device.concentration_level = device.last_concentration_level = DEVICE_CONCENTRATION_UNKNOWN;
	// Set default FSM state
	FSM_state = FSM_SYSTEM_STATE_STARTUP;
	// Flush FSM timer
	ResetTimer(TIMER_SYSTEM);
}

void FSM_SYSTEM_Process(void)
{
	switch(FSM_state) {

		case FSM_SYSTEM_STATE_IDLE: {
			/************************************************************************/
			/* Button processing                                                    */
			/************************************************************************/
			if(GetMessage(MSG_BTN_KEY_PRESSED)) { // <- Press message processing
				// Getting press type
				uint8_t *press_type = GetMessageParam(MSG_BTN_KEY_PRESSED);
				// Processing button press by press time
				if(*press_type == BUTTON_EVENT_LONG_PRESS) {
					if(device.mode != DEVICE_MODE_SHOW_MENU) {
						// Set device mode
						device.mode = DEVICE_MODE_SHOW_MENU;
						device.menu_cursor=0;
						MENU_Start();
						SendMessageWOParam(MSG_LCD_REFRESH_DISPLAY);
					}
				} else {
					// Processing key press action by device modes
					switch(device.mode) {

						case DEVICE_MODE_SHOW_MENU: {
							switch(MENU_SELECT) {
								case MENU_ACTION_CANCEL: { // <- EXIT menu action
									device.mode = DEVICE_MODE_IDLE;
									break;
								}

								default: break;
							}

							device.menu_cursor=0;
							MENU_Change(MENU_CHILD);
							SendMessageWOParam(MSG_LCD_REFRESH_DISPLAY);
							return;
						}




						default: break;
					}
				}
			}

			/************************************************************************/
			/* Encoder processing                                                   */
			/************************************************************************/
			if(GetMessage(MSG_ENC_ROTATE)) {
				// Get rotate direction
				int8_t *rotate = GetMessageParam(MSG_ENC_ROTATE);
				// Processing rotate action by device modes
				switch(device.mode) {
					// SHOW MENU mode rotate processing
					case DEVICE_MODE_SHOW_MENU: {
						if(*rotate > 0) {
							MENU_Change(MENU_NEXT);
						} else {
							MENU_Change(MENU_PREVIOUS);
						}
						// Send message to refresh display
						SendMessageWOParam(MSG_LCD_REFRESH_DISPLAY);
						break;
					}


					default: break;
				}

			}

			/************************************************************************/
			/* INDICATION CONTROL                                                   */
			/************************************************************************/
			if(!mhz19b._error && GetTimer(TIMER_SYSTEM) >= 1000) {
				//
				FSM_state = FSM_SYSTEM_STATE_CONCENTRATION_PROCESSING;
				ResetTimer(TIMER_SYSTEM);
				return;
			}

			/************************************************************************/
			/* INDICATION CONTROL                                                   */
			/************************************************************************/
			if(device.concentration_level != device.last_concentration_level) {
				// Store last concentration state
				device.last_concentration_level = device.concentration_level;
				// Activate led
				led_params._active = 1;
				// Set glow time to infinity
				led_params.glow_time_ms = 0xFFFF;
				switch(device.concentration_level) {
					case DEVICE_CONCENTRATION_NORMAL: {
						// Blinking - NO, Color - GREEN
						led_params._blinking = 0;
						led_params.color = GREEN;
						break;
					}

					case DEVICE_CONCENTRATION_NORMAL_ABOVE: {
						// Blinking - YES, Color - GREEN
						led_params._blinking = 1;
						led_params.color = GREEN;
						break;
					}

					case DEVICE_CONCENTRATION_MIDDLE: {
						// Blinking - NO, Color - YELLOW
						led_params._blinking = 0;
						led_params.color = YELLOW;
						break;
					}

					case DEVICE_CONCENTRATION_MIDDLE_ABOVE: {
						// Blinking - YES, Color - YELLOW
						led_params._blinking = 1;
						led_params.color = YELLOW;
						break;
					}

					case DEVICE_CONCENTRATION_HIGH: {
						// Blinking - NO, Color - RED
						led_params._blinking = 0;
						led_params.color = RED;
						break;
					}

					default: break;
				}
				// Send message with params
				SendMessageWParam(MSG_LEDS_PROCESSING, (void *)&led_params);
				ResetTimer(TIMER_SYSTEM);
			}

			return;
		}

		case FSM_SYSTEM_STATE_STARTUP: {
			// Waiting for system will be initialized
			if(GetTimer(TIMER_SYSTEM) > SYSTEM_INITIALIZATION_TIMEOUT) {
				// Send BC message after
				SendBCMessage(MSG_BC_SYSTEM_STARTUP_RUN);
				// Goto waiting complete rules
				FSM_state = FSM_SYSTEM_STATE_STARTUP_COMPLETE;
			}
			return;
		}

		case FSM_SYSTEM_STATE_STARTUP_COMPLETE: {
			// Waiting splash screen will be hidden
			if(GetMessage(MSG_CHK_SENSORS_COMPLETE)) {
				// Send BC message after
				SendBCMessage(MSG_BC_SYSTEM_STARTUP_COMPLETE);
				// Goto IDLE state
				FSM_state = FSM_SYSTEM_STATE_IDLE;
				// Set default device mode
				device.mode = DEVICE_MODE_IDLE;
			}
			return;
		}

		case FSM_SYSTEM_STATE_CONCENTRATION_PROCESSING: {
			if(mhz19b.value <= 900) {
				device.concentration_level = DEVICE_CONCENTRATION_NORMAL;
			} else if(mhz19b.value > 900 && mhz19b.value < 1000) {
				device.concentration_level = DEVICE_CONCENTRATION_NORMAL_ABOVE;
			} else if(mhz19b.value >= 1000 && mhz19b.value < 1700) {
				device.concentration_level = DEVICE_CONCENTRATION_MIDDLE;
			} else if(mhz19b.value >= 1700 && mhz19b.value < 2000) {
				device.concentration_level = DEVICE_CONCENTRATION_MIDDLE_ABOVE;
			} else {
				device.concentration_level = DEVICE_CONCENTRATION_HIGH;
			}
			FSM_state = FSM_SYSTEM_STATE_IDLE;
			return;
		}


		default: break;
	}
}