/*
 * lcd.c
 *
 * Created: 23.01.2019 11:51:02
 *  Author: v.bandura
 */
#include "include/lcd.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;
static					uint8_t			splash_length = PCF8574_COLS;
static					uint8_t			current_row=0;
static					uint8_t			_colon_blink=0;
static					menu_item_t		*last_menu_item;
const					uint8_t			chart_table[]		PROGMEM = { 0x20, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7 };

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
#if (LCD_SUPPORT_CUSTOM_CHARS_SIZE > 1)
void FSM_LCD_CreateCustomChars(void);
uint8_t FSM_LCD_DrawCustomChar(char chr, uint8_t start_row, uint8_t start_col);
uint8_t FSM_LCD_AddCustomCharStringToXY(char * str, uint8_t start_row, uint8_t start_col);
uint8_t FSM_LCD_AddCustomCharStringToXYFromFlash(const char * str, uint8_t start_row, uint8_t start_col);
#endif
void FSM_LCD_CreateMenuChar(void);
void FSM_LCD_RedrawMenuItems(void);
void FSM_LCD_CreateChartsChar(void);
void FSM_LCD_DrawChart(uint8_t *data, uint8_t length, uint8_t hysteresis, uint8_t absolute_max, uint8_t start_row, uint8_t start_col, uint8_t direction);
void FSM_LCD_DrawChart16(uint16_t *data, uint8_t length, uint8_t hysteresis, uint16_t absolute_max, uint8_t start_row, uint8_t start_col, uint8_t direction);
void FSM_LCD_DrawChartVerticalLine(uint8_t value, uint8_t start_row, uint8_t col);

void FSM_LCD_Init(void)
{
	// Set next FSM state
	FSM_state = FSM_LCD_STATE_LCD_INIT;
	// Flush FSM timer
	ResetTimer(TIMER_LCD);
}

void FSM_LCD_Process(void)
{
	uint8_t i;
	uint8_t _presence;
	char buff[6];

	switch(FSM_state) {
		/* Waiting LCD init instruction will be send */
		case FSM_LCD_STATE_LCD_INIT: {
			if(GetBCMessage(MSG_BC_PCF8574_INIT_CMD_SEND_COMPLETE)) {
				// Load into display custom chars
				FSM_LCD_CreateCustomChars();
				// Goto SPLASH screen load
				FSM_state = FSM_LCD_STATE_SPLASH_DRAW;
			}
			return;
		}

		/* Draw splash screen on LCD */
		case FSM_LCD_STATE_SPLASH_DRAW: {
			//   ### ###2   HW: x.x
			//   #   # #    SW: x.x
			//   ### ###
			//  SEMAPHORE
			FSM_LCD_AddCustomCharStringToXYFromFlash(LNG_SPLASH_LOGO_S1, 0, 3);
			FSM_PCF8574_AddStringFromFlashToXY(LNG_SPLASH_LOGO_S1_1, 0, 9);
			FSM_PCF8574_AddStringFromFlashToXY(LNG_SPLASH_LOGO_S4, 3, 2);
			FSM_PCF8574_AddStringFromFlashToXY(LNG_HW_VERSION, 0, 13);
			FSM_PCF8574_AddStringFromFlashToXY(LNG_SW_VERSION, 1, 13);
			// Prepare SPLASH show timeout countdown
			ResetTimer(TIMER_LCD);
			// Goto waiting SHOW timeout
			FSM_state = FSM_LCD_STATE_SPLASH_SHOW;
			return;
		}

		/* Splash show delay */
		case FSM_LCD_STATE_SPLASH_SHOW: {
			// Waiting for SPLASH showing timeout
			if(GetTimer(TIMER_LCD) > LCD_SPLASH_SPLASH_HIDE_TIMEOUT) {
				// Goto SPLASH hide animation
				FSM_state = FSM_LCD_STATE_SPLASH_HIDE_ANIMATION;
			}
			return;
		}

		/* Animated splash hiding */
		case FSM_LCD_STATE_SPLASH_HIDE_ANIMATION: {
			// Animation timeout
			if(GetTimer(TIMER_LCD) >= LCD_SPLASH_SLIDE_PERIOD) {
				if(splash_length--) {
					for(i=0; i<4; i++) {
						FSM_PCF8574_GoToXY(i,splash_length);
						FSM_PCF8574_AddRAWChar(0x20);
					}
					// Flush FSM timer
					ResetTimer(TIMER_LCD);
					return;
				}
				// When last position is 0, send command to change shift source to cursor
				FSM_PCF8574_AddByteToQueue(PCF8574_OPT_SHIFT_CURSOR | PCF8574_OPT_SHIFT_LEFT, PCF8574_COMMAND, PCF8574_BYTE_FULL, 1);
				// Clear display after animation
				FSM_PCF8574_Clear();
				// Goto sensor check state
				FSM_state = FSM_LCD_STATE_DRAW_SENSORS_STATE_SCREEN;
			}
			return;
		}

		/* Draw sensors check screen */
		case FSM_LCD_STATE_DRAW_SENSORS_STATE_SCREEN: {
			// Peripheral presence:
			//  -> RTC ...
			//  -> DHT ...
			//  -> CO2 ...
			FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_SCHK, 0, 0);
			// Goto sensors test result
			FSM_state = FSM_LCD_STATE_ANIMATION_SENSORS_STATE_SCREEN;
			// Flush FSM timer for process animation
			ResetTimer(TIMER_LCD);
			return;
		}

		/* Animation sensors check list */
		case FSM_LCD_STATE_ANIMATION_SENSORS_STATE_SCREEN: {
			uint16_t curr_timeout = 500;
			if(current_row == 2) {
				curr_timeout = MHZ19B_REFRESH_PERIOD + 1000;
			}

			/* Animate row by row with delay */
			if(GetTimer(TIMER_LCD) > curr_timeout) {
				// Showing sensors state
				switch(++current_row) {
					/* RTC sensor */
					case 1: {
						_presence = rtc._presence;
						break;
					}
					/* DHT sensor */
					case 2: {
						if(!dht._failed) {
							_presence = 1;
						} else {
							_presence = 0;
						}
						break;
					}
					/* CO2 sensor */
					case 3: {
						if(!mhz19b._error) {
							_presence=1;
						} else {
							_presence=0;
						}
						break;
					}
					/* Exit to idle state */
					default: {
						// Send message for complete startup detect
						SendMessageWOParam(MSG_CHK_SENSORS_COMPLETE);
						FSM_state = FSM_LCD_STATE_IDLE;
						ResetTimer(TIMER_LCD_BL);
						return;
					}
				}
				/* Draw state */
				if(_presence) {
					FSM_PCF8574_AddStringFromFlashToXY(LNG_OK, current_row, PCF8574_COLS-2);
				} else {
					FSM_PCF8574_AddStringFromFlashToXY(LNG_FAIL, current_row, PCF8574_COLS-4);
				}

				// Flush FSM timer for process animations
				ResetTimer(TIMER_LCD);
			}
			return;
		}

		case FSM_LCD_STATE_REFRESH_SCREEN: {
			uint8_t _full_refresh = (device.mode != device.last_mode);
			device.last_mode = device.mode;

			switch(device.mode) {

				case DEVICE_MODE_IDLE: {
					// @TODO: Need optimization

					// If mode was changed - full refresh LCD screen
					if(_full_refresh) {
						// Clear display
						FSM_PCF8574_Clear();
						// Upload custom characters into LCD
						FSM_LCD_CreateCustomChars();
						// Load first line of splash string to display
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_MAIN, 0, PCF8574_COLS-3);
					}

					// Draw big clock data
					uint8_t pos = FSM_LCD_AddCustomCharStringToXY(utoa_cycle_sub8(rtc.hour, buff, 1, 2), 0, 0);
					FSM_LCD_AddCustomCharStringToXY(utoa_cycle_sub8(rtc.min, buff, 0, 2), 0, ++pos);
					// Draw blinking colon
					if((_colon_blink ^= 0x1)) {
						FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_COLON, 1, 6);
					} else {
						FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_SPACE, 1, 6);
					}

					// Draw date data
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(rtc.day, buff, 0, 2), 0, PCF8574_COLS-5);
					//FSM_PCF8574_AddStringFromFlash(LNG_SMB_POINT, 0, PCF8574_COLS-3);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(rtc.month, buff, 0, 2), 0, PCF8574_COLS-2);
					FSM_PCF8574_AddStringFromFlashToXY((char *)LNG_DOW[rtc.dow], 1, PCF8574_COLS-4);
					// Draw T data
					if(dht.temperature.sign) {
						FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_MINUS, 3, 0);
					} else {
						FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_SPACE, 3, 0);
					}
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(dht.temperature.value, buff, 1, 2), 3, 1);
					// Draw H data
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(dht.humidity.value, buff, 1, 2), 3, 7);
					// Draw CO2 data
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub16(mhz19b.value, buff, 1, 4), 3, 13);
					if(mhz19b._error) {
						FSM_PCF8574_AddStringFromFlashToXY(PSTR("!"), 3, 12);
					} else {
						FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_SPACE, 3, 12);
					}

					// /@TODO: Need optimization
					break;
				}

				case DEVICE_MODE_IDLE_CHART_CO2_HOURLY: {
					// If mode was changed - full refresh LCD screen
					if(_full_refresh) {
						// Clear display
						FSM_PCF8574_Clear();
						// Upload charts char into LCD
						FSM_LCD_CreateChartsChar();
						// Load first line of splash string to display
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_CHART_CO2_HOURLY, 0, 0);
						// Drawing chart procedure
						FSM_LCD_DrawChart16((void *)&device.charts.co2.by_minute, 15, 200, 5000, 3, 0, 0);
					}
					break;
				}

				case DEVICE_MODE_IDLE_CHART_CO2_DAILY: {
					// If mode was changed - full refresh LCD screen
					if(_full_refresh) {
						// Clear display
						FSM_PCF8574_Clear();
						// Upload charts char into LCD
						FSM_LCD_CreateChartsChar();
						// Load first line of splash string to display
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_CHART_CO2_DAILY, 0, 0);
						// Drawing chart procedure
						FSM_LCD_DrawChart16((void *)&device.charts.co2.by_hour, 15, 200, 5000, 3, 0, 0);
					}
					break;
				}

				case DEVICE_MODE_IDLE_CHART_HUMIDITY_HOURLY: {
					// If mode was changed - full refresh LCD screen
					if(_full_refresh) {
						// Clear display
						FSM_PCF8574_Clear();
						// Load first line of splash string to display
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_CHART_H_HOURLY, 0, 0);
						// Drawing chart procedure
						FSM_LCD_DrawChart((void *)&device.charts.h.by_minute, 15, 10, 99, 3, 0, 0);
					}
					break;
				}

				case DEVICE_MODE_IDLE_CHART_HUMIDITY_DAILY: {
					// If mode was changed - full refresh LCD screen
					if(_full_refresh) {
						// Clear display
						FSM_PCF8574_Clear();
						// Load first line of splash string to display
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_CHART_H_DAILY, 0, 0);
						// Drawing chart procedure
						FSM_LCD_DrawChart((void *)&device.charts.h.by_hour, 15, 10, 99, 3, 0, 0);
					}
					break;
				}

				case DEVICE_MODE_IDLE_CHART_TEMPERATURE_HOURLY: {
					// If mode was changed - full refresh LCD screen
					if(_full_refresh) {
						// Clear display
						FSM_PCF8574_Clear();
						// Load first line of splash string to display
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_CHART_T_HOURLY, 0, 0);
						// Drawing chart procedure
						FSM_LCD_DrawChart((void *)&device.charts.t.by_minute, 15, 10, 50, 3, 0, 0);
					}
					break;
				}

				case DEVICE_MODE_IDLE_CHART_TEMPERATURE_DAILY: {
					// If mode was changed - full refresh LCD screen
					if(_full_refresh) {
						// Clear display
						FSM_PCF8574_Clear();
						// Load first line of splash string to display
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_CHART_T_DAILY, 0, 0);
						// Drawing chart procedure
						FSM_LCD_DrawChart((void *)&device.charts.t.by_hour, 15, 10, 50, 3, 0, 0);
					}
					break;
				}

				case DEVICE_MODE_SHOW_MENU: {
					// If mode was changed - recreate menu chars icon
					if(_full_refresh) {
						// Upload menu custom chars
						FSM_LCD_CreateMenuChar();
						// Redrawing menu items
						FSM_LCD_RedrawMenuItems();
					}
					if(device.flags._menu_changed) {
						// Flush menu change flag
						device.flags._menu_changed = 0;

						// Cursor movement procedure
						if((menu_item_t *)pgm_read_word(&last_menu_item->Previous) == (void *)selected_menu_item) {
							if(device.menu_cursor > 0) {
								device.menu_cursor--;
							}
						}
						if((menu_item_t *)pgm_read_word(&last_menu_item->Next) == (void *)selected_menu_item) {
							if(device.menu_cursor < PCF8574_ROWS-1) {
								device.menu_cursor++;
							}
						}
						/* Redraw display if need */
						if(
							// Cursor in 0 row position and current menu item have previous menu item
							(!device.menu_cursor && (void *)pgm_read_word(&selected_menu_item->Previous) != (void *)&NULL_ENTRY) ||
							// Cursor in max row position and current menu item have next menu item
							(device.menu_cursor == 4 && (void *)pgm_read_word(&selected_menu_item->Next) != (void *)&NULL_ENTRY) ||
							// Detect that menu deep was changed
							((void *)pgm_read_word(&last_menu_item->Child) != (void *)&NULL_ENTRY && (void *)pgm_read_word(&last_menu_item->Child) == (void *)selected_menu_item) ||
							((void *)pgm_read_word(&last_menu_item->Parent) != (void *)&NULL_ENTRY && (void *)pgm_read_word(&last_menu_item->Parent) == (void *)selected_menu_item)
						) {
							// Do screen update
							FSM_LCD_RedrawMenuItems();
						}
						// Draw menu on the LCD line by line
						for(uint8_t i=0; i < PCF8574_ROWS; i++) {
							// Draw cursor
							if(device.menu_cursor == i) {
								FSM_PCF8574_AddRAWCharFromFlashToXY(PSTR("\x1"), i, 0);
							} else {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_SPACE, i, 0);
							}
						}
						// Store last menu item pointer
						last_menu_item = selected_menu_item;
					}
					break;
				}

				case DEVICE_MODE_DATETIME_SET_DATE: {
					if(device.flags._idx_changed) { // <- Index was changed - full refresh
						// Flush idx changed flag
						device.flags._idx_changed=0;
						// Prepare display static data
						//     Setup date:
						//
						//	   Fri 01/01/00
						//     Save?         Yes No
						FSM_PCF8574_Clear();
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_SETUP_DATE, 0, 0);
						FSM_PCF8574_AddStringFromFlashToXY(LNG_YES, PCF8574_ROWS-1, PCF8574_COLS-6);
						FSM_PCF8574_AddStringFromFlashToXY(LNG_NO, PCF8574_ROWS-1, PCF8574_COLS-2);

						// Draw cursor position
						switch(device.idx_curr) {

							case 1: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_DBL_UNDERSCORE, 1, 4);
								break;
							}

							case 2: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_DBL_UNDERSCORE, 1, 7);
								break;
							}

							case 3: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_DBL_UNDERSCORE, 1, 10);
								break;
							}

							case 4: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, PCF8574_ROWS-1, PCF8574_COLS-7);
								break;
							}

							case 5: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, PCF8574_ROWS-1, PCF8574_COLS-3);
								break;
							}

							default: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_DBL_UNDERSCORE, 1, 1);
								break;
							}
						}
					}
					// Draw DATE data
					FSM_PCF8574_AddStringFromFlashToXY((char *)LNG_DOW[rtc.dow], 2, 0);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(rtc.day, buff, 0, 2), 2, 4);
					FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_SLASH, 2, 6);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(rtc.month, buff, 0, 2), 2, 7);
					FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_SLASH, 2, 9);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(rtc.year, buff, 0, 2), 2, 10);
					break;
				}

				case DEVICE_MODE_DATETIME_SET_TIME: {
					if(device.flags._idx_changed) { // <- Index was changed - full refresh
						// Flush idx changed flag
						device.flags._idx_changed=0;
						// Prepare display static data
						//     Setup time:
						//
						//	   00:00:00
						//     Save?         Yes No
						FSM_PCF8574_Clear();
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_SETUP_TIME, 0, 0);
						FSM_PCF8574_AddStringFromFlashToXY(LNG_YES, PCF8574_ROWS-1, PCF8574_COLS-6);
						FSM_PCF8574_AddStringFromFlashToXY(LNG_NO, PCF8574_ROWS-1, PCF8574_COLS-2);

						// Draw cursor position
						switch(device.idx_curr) {

							case 1: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_DBL_UNDERSCORE, 1, 3);
								break;
							}

							case 2: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_DBL_UNDERSCORE, 1, 6);
								break;
							}

							case 3: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, PCF8574_ROWS-1, PCF8574_COLS-7);
								break;
							}

							case 4: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, PCF8574_ROWS-1, PCF8574_COLS-3);
								break;
							}

							default: {
								FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_DBL_UNDERSCORE, 1, 0);
								break;
							}
						}
					}
					// Draw TIME data
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(rtc.hour, buff, 0, 2), 2, 0);
					FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_COLON, 2, 2);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(rtc.min, buff, 0, 2), 2, 3);
					FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_COLON, 2, 5);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(rtc.sec, buff, 0, 2), 2, 6);
					break;
				}

				case DEVICE_MODE_LCD_BL_SET: {
					if(device.flags._idx_changed) { // <- Index was changed - full refresh
						// Flush idx changed flag
						device.flags._idx_changed=0;
						// Prepare display static data
						//     Default            8
						//     By time            5
						//     Range    14.00-15.00
						//     Save?         Yes No
						FSM_PCF8574_Clear();
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_SETUP_LCD_BL, 0, 0);
						FSM_PCF8574_AddStringFromFlashToXY(LNG_YES, PCF8574_ROWS-1, PCF8574_COLS-6);
						FSM_PCF8574_AddStringFromFlashToXY(LNG_NO, PCF8574_ROWS-1, PCF8574_COLS-2);
					}

					// Draw default bl pwm level
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.lcd.bl_pwm_default, buff, 1, 2), 0, PCF8574_COLS-2);
					// Draw bl pwm level by time range
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.lcd.bl_pwm_by_time, buff, 1, 2), 1, PCF8574_COLS-2);
					// Draw time range
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.lcd.from_hour, buff, 1, 2), 2, PCF8574_COLS-11);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.lcd.from_min, buff, 0, 2), 2, PCF8574_COLS-8);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.lcd.to_hour, buff, 1, 2), 2, PCF8574_COLS-5);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.lcd.to_min, buff, 0, 2), 2, PCF8574_COLS-2);

					// Draw cursor position
					switch(device.idx_curr) {

						case 1: {
							FSM_PCF8574_GoToXY(1, PCF8574_COLS-1);
							break;
						}

						case 2: {
							FSM_PCF8574_GoToXY(2, PCF8574_COLS-10);
							break;
						}

						case 3: {
							FSM_PCF8574_GoToXY(2, PCF8574_COLS-7);
							break;
						}

						case 4: {
							FSM_PCF8574_GoToXY(2, PCF8574_COLS-4);
							break;
						}

						case 5: {
							FSM_PCF8574_GoToXY(2, PCF8574_COLS-1);
							break;
						}

						case 6: {
							FSM_PCF8574_AddByteToQueue(PCF8574_CMD_DISPLAY_MODE | PCF8574_OPT_DISPLAY_ENABLE | PCF8574_OPT_CURSOR_INVISIBLE, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
							FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, PCF8574_ROWS-1, PCF8574_COLS-7);
							break;
						}

						case 7: {
							FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, PCF8574_ROWS-1, PCF8574_COLS-3);
							break;
						}

						default: {
							FSM_PCF8574_AddByteToQueue(PCF8574_CMD_DISPLAY_MODE | PCF8574_OPT_DISPLAY_ENABLE | PCF8574_OPT_CURSOR_VISIBLE | PCF8574_OPT_CURSOR_IS_UNDERLINE, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
							FSM_PCF8574_GoToXY(0, PCF8574_COLS-1);
							break;
						}
					}
					break;
				}

				case DEVICE_MODE_BUZZER_SET: {
					if(device.flags._idx_changed) { // <- Index was changed - full refresh
						// Flush idx changed flag
						device.flags._idx_changed=0;
						// Prepare display static data
						//     Default           On
						//     By time          Off
						//     Range    14.00-15.00
						//     Save?         Yes No
						FSM_PCF8574_Clear();
						FSM_PCF8574_AddStringFromFlashToXY(LNG_DM_SETUP_BUZZER, 0, 0);
						FSM_PCF8574_AddStringFromFlashToXY(LNG_YES, PCF8574_ROWS-1, PCF8574_COLS-6);
						FSM_PCF8574_AddStringFromFlashToXY(LNG_NO, PCF8574_ROWS-1, PCF8574_COLS-2);
					}

					// Draw default buzzer state
					if(device.settings.buzzer._default_state) {
						FSM_PCF8574_AddStringFromFlashToXY(LNG_ON, 0, PCF8574_COLS-3);
					} else {
						FSM_PCF8574_AddStringFromFlashToXY(LNG_OFF, 0, PCF8574_COLS-3);
					}
					// Draw by time buzzer state
					if(device.settings.buzzer._bytime_state) {
						FSM_PCF8574_AddStringFromFlashToXY(LNG_ON, 1, PCF8574_COLS-3);
					} else {
						FSM_PCF8574_AddStringFromFlashToXY(LNG_OFF, 1, PCF8574_COLS-3);
					}
					// Draw time range
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.buzzer.from_hour, buff, 1, 2), 2, PCF8574_COLS-11);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.buzzer.from_min, buff, 0, 2), 2, PCF8574_COLS-8);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.buzzer.to_hour, buff, 1, 2), 2, PCF8574_COLS-5);
					FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(device.settings.buzzer.to_min, buff, 0, 2), 2, PCF8574_COLS-2);

					// Draw cursor position
					switch(device.idx_curr) {

						case 1: {
							FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, 1, PCF8574_COLS-4);
							break;
						}

						case 2: {
							FSM_PCF8574_AddByteToQueue(PCF8574_CMD_DISPLAY_MODE | PCF8574_OPT_DISPLAY_ENABLE | PCF8574_OPT_CURSOR_VISIBLE, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
							FSM_PCF8574_GoToXY(2, PCF8574_COLS-10);
							break;
						}

						case 3: {
							FSM_PCF8574_GoToXY(2, PCF8574_COLS-7);
							break;
						}

						case 4: {
							FSM_PCF8574_GoToXY(2, PCF8574_COLS-4);
							break;
						}

						case 5: {
							FSM_PCF8574_GoToXY(2, PCF8574_COLS-1);
							break;
						}

						case 6: {
							FSM_PCF8574_AddByteToQueue(PCF8574_CMD_DISPLAY_MODE | PCF8574_OPT_DISPLAY_ENABLE | PCF8574_OPT_CURSOR_INVISIBLE, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
							FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, PCF8574_ROWS-1, PCF8574_COLS-7);
							break;
						}

						case 7: {
							FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, PCF8574_ROWS-1, PCF8574_COLS-3);
							break;
						}

						default: {
							FSM_PCF8574_AddStringFromFlashToXY(LNG_SMB_ANGLE_BRACKET_RIGHT, 0, PCF8574_COLS-4);
							break;
						}
					}

					break;
				}


				default: {
					FSM_PCF8574_Clear();
					break;
				}
			}
			// Flush FSM timer
			ResetTimer(TIMER_LCD);
			// Goto idle state
			FSM_state = FSM_LCD_STATE_IDLE;
			return;
		}

		/* Default work state */
		case FSM_LCD_STATE_IDLE: {
			if(GetTimer(TIMER_LCD) >= LCD_REFRESH_PERIOD || GetMessage(MSG_LCD_REFRESH_DISPLAY)) {
				// Set next FSM state
				FSM_state = FSM_LCD_STATE_REFRESH_SCREEN;
				ResetTimer(TIMER_LCD);
			}
			return;
		}

		default: break;
	}
}


#if (LCD_SUPPORT_CUSTOM_CHARS_SIZE > 1)
void FSM_LCD_CreateCustomChars(void)
{
	FSM_PCF8574_CreateCharacterFromFlash(0, cc_smb1);
	FSM_PCF8574_CreateCharacterFromFlash(1, cc_smb2);
	FSM_PCF8574_CreateCharacterFromFlash(2, cc_smb3);
	FSM_PCF8574_CreateCharacterFromFlash(3, cc_smb4);
	FSM_PCF8574_CreateCharacterFromFlash(4, cc_smb5);
	FSM_PCF8574_CreateCharacterFromFlash(5, cc_smb6);
	FSM_PCF8574_CreateCharacterFromFlash(6, cc_smb7);
	FSM_PCF8574_CreateCharacterFromFlash(7, cc_smb8);
}

uint8_t FSM_LCD_DrawCustomChar(char chr, uint8_t start_row, uint8_t start_col)
{
	const char * pChr = NULL;
	char c;
	uint8_t		i=start_row, j=start_col;

	//  Symbol converting table
	switch(chr) {
		case '0': { pChr = cc_char_0; break; }
		case '1': { pChr = cc_char_1; break; }
		case '2': { pChr = cc_char_2; break; }
		case '3': { pChr = cc_char_3; break; }
		case '4': { pChr = cc_char_4; break; }
		case '5': { pChr = cc_char_5; break; }
		case '6': { pChr = cc_char_6; break; }
		case '7': { pChr = cc_char_7; break; }
		case '8': { pChr = cc_char_8; break; }
		case '9': { pChr = cc_char_9; break; }
		case 'A': case 'a': { pChr = cc_char_a; break; }
		case 'B': case 'b': { pChr = cc_char_b; break; }
		case 'C': case 'c': { pChr = cc_char_c; break; }
		case 'D': case 'd': { pChr = cc_char_d; break; }
		case 'E': case 'e': { pChr = cc_char_e; break; }
		case 'F': case 'f': { pChr = cc_char_f; break; }
		case ':': { pChr = cc_char_colon; break; }
		case ' ': { pChr = cc_char_space; break; }
		default: break;
	}

	if(pChr != NULL) {
		// Go to start position ROW & COL
		FSM_PCF8574_GoToXY(i,j);
		// Draw processing line by line
		while((c = pgm_read_byte(pChr)) != '$') {
			if(c == '&') { // <- Next line sybol
				// Increment ROW position
				i++;
				// Store COL position
				j=start_col;
				// Go to next ROW
				FSM_PCF8574_GoToXY(i, j);
			} else {
				// Change col position to the end
				j++;
				// Draw char
				FSM_PCF8574_AddRAWChar(c);
			}
			pChr++;
		}
	}
	// Return last position by Y axis with char size include
	return j;
}


uint8_t FSM_LCD_AddCustomCharStringToXY(char * str, uint8_t start_row, uint8_t start_col)
{
	uint8_t pos_col = start_col;

	while(*str) {
		// Store col position for the next SYMBOL
		pos_col = FSM_LCD_DrawCustomChar(*str, start_row, pos_col);
		str++;
	}
	// Return last column position
	return pos_col;
}

uint8_t FSM_LCD_AddCustomCharStringToXYFromFlash(const char * str, uint8_t start_row, uint8_t start_col)
{
	char chr;
	uint8_t pos_col = start_col;

	while((chr = pgm_read_byte(str))) {
		// Store col position for the next SYMBOL
		pos_col = FSM_LCD_DrawCustomChar(chr, start_row, pos_col);
		str++;
	}
	// Return last column position
	return pos_col;
}
#endif

void FSM_LCD_CreateMenuChar(void)
{
	FSM_PCF8574_CreateCharacterFromFlash(0, cc_icon_enter);
	FSM_PCF8574_CreateCharacterFromFlash(1, cc_icon_solid_rarrow);
	FSM_PCF8574_CreateCharacterFromFlash(2, cc_icon_top);
}

void FSM_LCD_RedrawMenuItems(void)
{
	// Clear display
	FSM_PCF8574_Clear();
	// Draw menu on the LCD line by line
	for(uint8_t i=0; i < PCF8574_ROWS; i++) {
		// Draw menu item title
		uint8_t shift = i-device.menu_cursor;
		FSM_PCF8574_AddStringFromFlashToXY(MENU_GetMenuTextByShift(shift), i, 1);
		// Draw subcategory symbol
		FSM_PCF8574_AddStringFromFlashToXY(MENU_GetChildMenuSymbolByShift(shift), i, PCF8574_COLS-1);
	}
}

/************************************************************************/
/* Chart functions                                                      */
/************************************************************************/
void FSM_LCD_CreateChartsChar(void)
{
	FSM_PCF8574_CreateCharacterFromFlash(0, level0);
	FSM_PCF8574_CreateCharacterFromFlash(1, level1);
	FSM_PCF8574_CreateCharacterFromFlash(2, level2);
	FSM_PCF8574_CreateCharacterFromFlash(3, level3);
	FSM_PCF8574_CreateCharacterFromFlash(4, level4);
	FSM_PCF8574_CreateCharacterFromFlash(5, level5);
	FSM_PCF8574_CreateCharacterFromFlash(6, level6);
	FSM_PCF8574_CreateCharacterFromFlash(7, level7);
}

void FSM_LCD_DrawChart(uint8_t *data, uint8_t length, uint8_t hysteresis, uint8_t absolute_max, uint8_t start_row, uint8_t start_col, uint8_t direction)
{
	uint8_t next_col;
	uint8_t curr_value;
	uint8_t *p_buff = data;
	char buff[4];
	uint8_t min = 0;
	uint8_t max = 0;

	/* Scan buffer for min and max values */
	for(uint8_t i=0; i<length; i++) {
		// Setup min value
		if(!min || min > *p_buff) { min = *p_buff; }
		// Setup max value
		if(max < *p_buff) { max = *p_buff; }
		p_buff++;
	}
	/* Hysteresis */
	if(min > hysteresis) { min -= hysteresis; } else { min = 0; }
	max += hysteresis;
	if(absolute_max < max) { max = absolute_max; }

	// Scale calculating
	uint8_t scale = ((max - min) / 24);
	uint8_t scale_mantissa = (max-min) % 24;
	if(scale) { // <- Scale more then 1
		// Analog Round() function
		if(scale_mantissa > 5) { scale++; }
	} else { // <- Scale is zero
		scale = 1;
	}
	// Draw min and max value
	FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(max, buff, 1, 2), 1, PCF8574_COLS-2);
	FSM_PCF8574_AddStringToXY(utoa_cycle_sub8(min, buff, 1, 2), 3, PCF8574_COLS-2);
	// Main draw mechanism
	for(uint8_t i=0; i<length; i++) {
		if(direction) { // <- Direction from right to the left
			next_col = start_col - i;
		} else { // <- Direction from left to the right
			next_col = start_col + i;
		}
		// Calculate scaled value
		curr_value = (data[i] - min) / scale;
		// Draw column
		FSM_LCD_DrawChartVerticalLine(curr_value, 3, next_col);
	}
}

void FSM_LCD_DrawChart16(uint16_t *data, uint8_t length, uint8_t hysteresis, uint16_t absolute_max, uint8_t start_row, uint8_t start_col, uint8_t direction)
{
	uint8_t next_col;
	uint8_t curr_value;
	uint16_t *p_buff = data;
	char buff[6];
	uint16_t min = 0;
	uint16_t max = 0;

	/* Scan buffer for min and max values */
	for(uint8_t i=0; i<length; i++) {
		// Setup min value
		if(!min || min > *p_buff) { min = *p_buff; }
		// Setup max value
		if(max < *p_buff) { max = *p_buff; }
		p_buff++;
	}

	/* Hysteresis */
	if(min > hysteresis) { min -= hysteresis; } else { min = 0; }
	max += hysteresis;
	if(absolute_max < max) { max = absolute_max; }

	// Scale calculating
	uint16_t scale = ((max - min) / 24);
	uint16_t scale_mantissa = (max-min) % 24;
	if(scale) { // <- Scale more then 1
		// Analog Round() function
		if(scale_mantissa > 5) { scale++; }
	} else { // <- Scale is zero
		scale = 1;
	}
	// Draw min and max value
	FSM_PCF8574_AddStringToXY(utoa_cycle_sub16(max, buff, 1, 4), 1, PCF8574_COLS-4);
	FSM_PCF8574_AddStringToXY(utoa_cycle_sub16(min, buff, 1, 4), 3, PCF8574_COLS-4);
	// Main draw mechanism
	for(uint8_t i=0; i<length; i++) {
		if(direction) { // <- Direction from right to the left
			next_col = start_col - i;
		} else { // <- Direction from left to the right
			next_col = start_col + i;
		}
		// Calculate scaled value
		curr_value = (data[i] - min) / scale;
		// Draw column
		FSM_LCD_DrawChartVerticalLine(curr_value, 3, next_col);
	}
}

void FSM_LCD_DrawChartVerticalLine(uint8_t value, uint8_t start_row, uint8_t col)
{
	uint8_t p_char;
	uint8_t curr_row = start_row;
	//uint8_t curr_value = value;

	// Calculate scaled value for drawing
	for(uint8_t i=0; i<3; i++) {
		if(value >= 8) {
			value -= 8;
			p_char = pgm_read_byte(chart_table + 8);
		} else {
			p_char = pgm_read_byte(chart_table + value);
			value = 0;
		}
		FSM_PCF8574_AddRAWCharToXY(p_char, curr_row, col);
		curr_row--;
	}
}