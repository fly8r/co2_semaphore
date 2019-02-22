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
static					buzzer_data_t	buzzer_params;

/* Max values for DATE setup */
const uint8_t	min_date_values[]	PROGMEM = {1, 1, 1, 0};	// Dow, Day, Month, Year
const uint8_t	max_date_values[]	PROGMEM = {7,31,12,99}; // Dow, Day, Month, Year
/* Max values for TIME setup */
const uint8_t	min_time_values[]	PROGMEM = { 0, 0, 0};		// Hour, min, sec
const uint8_t	max_time_values[]	PROGMEM = {23,59,59};	// Hour, min, sec


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

								case MENU_ACTION_DATETIME_SET_DATE: {
									// Send message to RTC module that stop
									// read data from chip
									SendMessageWOParam(MSG_RTC_STOP_READ);
									// Change device mode
									device.mode = DEVICE_MODE_DATETIME_SET_DATE;
									// Set data IDX change flag for refresh screen
									device.flags._idx_changed=1;
									break;
								}

								case MENU_ACTION_DATETIME_SET_TIME: {
									// Send message to RTC module that stop
									// read data from chip
									SendMessageWOParam(MSG_RTC_STOP_READ);
									// Change device mode
									device.mode = DEVICE_MODE_DATETIME_SET_TIME;
									// Set data IDX change flag for refresh screen
									device.flags._idx_changed=1;
									break;
								}


								default: break;
							}

							device.menu_cursor=0;
							MENU_Change(MENU_CHILD);
							SendMessageWOParam(MSG_LCD_REFRESH_DISPLAY);
							return;
						}

						case DEVICE_MODE_DATETIME_SET_DATE: {
							// Set data IDX change flag for refresh screen
							device.flags._idx_changed=1;
							// Increment current data index
							if(device.idx_curr++ > 3) {
								// Action by answer for question SAVE Y(4) N(5)
								if(device.idx_curr == 5) { // <- Y - yes
									// Send message for save date data
									SendMessageWOParam(MSG_RTC_SET_DATE);
								} else { // <- N - no
									// Send message for continue read data from RTC
									SendMessageWOParam(MSG_RTC_RESUME_READ);
								}
								// Flush current data index
								device.idx_curr=0;
								// Set current device mode
								device.mode = DEVICE_MODE_SHOW_MENU;
								// Redraw display flag when menu changed
								device.flags._menu_changed=1;
							}
							// Send command for refresh display
							SendMessageWOParam(MSG_LCD_REFRESH_DISPLAY);
							return;
						}

						case DEVICE_MODE_DATETIME_SET_TIME: {
							// Set data IDX change flag for refresh screen
							device.flags._idx_changed=1;
							// Increment current data index
							if(device.idx_curr++ > 2) {
								// Action by answer for question SAVE Y(4) N(5)
								if(device.idx_curr == 4) { // <- Y - yes
									// Send message for save date data
									SendMessageWOParam(MSG_RTC_SET_TIME);
								} else { // <- N - no
									// Send message for continue read data from RTC
									SendMessageWOParam(MSG_RTC_RESUME_READ);
								}
								// Flush current data index
								device.idx_curr=0;
								// Set current device mode
								device.mode = DEVICE_MODE_SHOW_MENU;
								// Redraw display flag when menu changed
								device.flags._menu_changed=1;
							}
							// Send command for refresh display
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
					// Setup date mode rotate processing
					case DEVICE_MODE_DATETIME_SET_DATE: {
						uint8_t * d;
						uint8_t min, max;
						// Get pointer to data DOW
						d = (void *)&rtc.dow;
						//  0   1  2  3  4 5    <- idx_curr
						// Fri 01/01/00
						//           	 Y N
						if(device.idx_curr > 3) {
							// Answer position change ->    Y   N
							if(device.idx_curr == 4) {
								device.idx_curr++;
							} else if(device.idx_curr == 5) {
								device.idx_curr--;
							}
							// Set idx change flag for display full refresh
							device.flags._idx_changed=1;
						} else {
							// Get data pointer for change
							d += device.idx_curr;
							// Get min/max value for current data pointer
							min = pgm_read_byte(min_date_values + device.idx_curr);
							max = pgm_read_byte(max_date_values + device.idx_curr);
							// Rotation processing
							if(*rotate > 0) {
								if(++(*d) > max) *d = min;
							} else {
								if(--(*d) < min || *d >= max) *d = max;
							}
						}
						// Send message to refresh display
						SendMessageWOParam(MSG_LCD_REFRESH_DISPLAY);
						break;
					}

					// Setup time mode rotate processing
					case DEVICE_MODE_DATETIME_SET_TIME: {
						uint8_t * d;
						uint8_t min, max;
						// Get pointer to data DOW
						d = (void *)&rtc.hour;
						//  0  1  2     3 4    <- idx_curr
						//              SAVE
						// 01:01:18	     Y N
						if(device.idx_curr > 2) {
							// Answer position change ->    Y   N
							if(device.idx_curr == 3) {
								device.idx_curr++;
								} else if(device.idx_curr == 4) {
								device.idx_curr--;
							}
							// Set idx change flag for display full refresh
							device.flags._idx_changed=1;
						} else {
							// Get data pointer for change
							d -= device.idx_curr;
							// Get min/max value for current data pointer
							min = pgm_read_byte(min_time_values + device.idx_curr);
							max = pgm_read_byte(max_time_values + device.idx_curr);
							// Rotation processing
							if(*rotate > 0) {
								if(++(*d) > max) *d = min;
							} else {
								if(--(*d) < min || *d >= max) *d = max;
							}
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

				// Activate led
				led_params._enable = 1;
				led_params._active = 1;
				// Enable buzzer control
				buzzer_params._enable = 1;
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
						// Turn on buzzer only if last concentration level was above normal
						if(device.last_concentration_level == DEVICE_CONCENTRATION_NORMAL_ABOVE) {
							buzzer_params._active = 1;
							buzzer_params.pulse_mode = BUZZER_PULSE_MODE_INTERVAL_EQUAL;
							buzzer_params.pulse_count = 3;
						}
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
						//
						buzzer_params._active = 1;
						buzzer_params.pulse_mode = BUZZER_PULSE_MODE_INTERVAL_NOT_EQUAL;
						buzzer_params.pulse_count = 9;
						break;
					}

					default: break;
				}
				// Store last concentration state
				device.last_concentration_level = device.concentration_level;
				// Send message with params
				SendMessageWParam(MSG_LEDS_PROCESSING, (void *)&led_params);
				SendMessageWParam(MSG_BUZZER_PROCESSING, (void *)&buzzer_params);
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