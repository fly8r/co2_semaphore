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

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
#if (LCD_SUPPORT_CUSTOM_CHARS_SIZE > 1)
void FSM_LCD_CreateCustomChars(void);
uint8_t FSM_LCD_DrawCustomChar(char chr, uint8_t start_row, uint8_t start_col);
uint8_t FSM_LCD_AddCustomCharStringToXY(char * str, uint8_t start_row, uint8_t start_col);
uint8_t FSM_LCD_AddCustomCharStringToXYFromFlash(const char * str, uint8_t start_row, uint8_t start_col);
#endif

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
			FSM_PCF8574_GoToXY(0,9);
			FSM_PCF8574_AddStringFromFlash(LNG_SPLASH_LOGO_S1_1);
			FSM_PCF8574_GoToXY(3,2);
			FSM_PCF8574_AddStringFromFlash(LNG_SPLASH_LOGO_S4);
			FSM_PCF8574_GoToXY(0,13);
			FSM_PCF8574_AddStringFromFlash(LNG_HW_VERSION);
			FSM_PCF8574_GoToXY(1,13);
			FSM_PCF8574_AddStringFromFlash(LNG_SW_VERSION);
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
			FSM_PCF8574_AddStringFromFlash(LNG_DM1_S1);
			FSM_PCF8574_AddStringFromFlash(LNG_DM1_S2);
			FSM_PCF8574_AddStringFromFlash(LNG_DM1_S3);
			FSM_PCF8574_AddStringFromFlash(LNG_DM1_S4);
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
						return;
					}
				}
				/* Draw state */
				if(_presence) {
					FSM_PCF8574_GoToXY(current_row, PCF8574_COLS-2);
					FSM_PCF8574_AddStringFromFlash(LNG_OK);
				} else {
					FSM_PCF8574_GoToXY(current_row, PCF8574_COLS-4);
					FSM_PCF8574_AddStringFromFlash(LNG_FAIL);
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
						// Load first line of splash string to display
						FSM_PCF8574_GoToXY(0, PCF8574_COLS-3);
						FSM_PCF8574_AddStringFromFlash(LNG_SMB_POINT);
						FSM_PCF8574_GoToXY(3, 0);
						FSM_PCF8574_AddStringFromFlash(LNG_DM_MAIN_S4);
					}

					// Draw date data
					FSM_PCF8574_GoToXY(0, PCF8574_COLS-5);
					FSM_PCF8574_AddString(utoa_cycle_sub8(rtc.day, buff, 0, 2));
					FSM_PCF8574_GoToXY(0, PCF8574_COLS-2);
					FSM_PCF8574_AddString(utoa_cycle_sub8(rtc.month, buff, 0, 2));
					FSM_PCF8574_GoToXY(1, PCF8574_COLS-4);
					FSM_PCF8574_AddStringFromFlash((char *)LNG_DOW[rtc.dow]);
					// Draw T data
					FSM_PCF8574_GoToXY(3, 0);
					if(dht.temperature.sign) {
						FSM_PCF8574_AddStringFromFlash(LNG_SMB_MINUS);
					} else {
						FSM_PCF8574_AddStringFromFlash(LNG_SMB_SPACE);
					}
					FSM_PCF8574_AddString(utoa_cycle_sub8(dht.temperature.value, buff, 1, 2));
					// Draw H data
					FSM_PCF8574_GoToXY(3, 7);
					FSM_PCF8574_AddString(utoa_cycle_sub8(dht.humidity.value, buff, 1, 2));
					// Draw CO2 data
					FSM_PCF8574_GoToXY(3,13);
					FSM_PCF8574_AddString(utoa_cycle_sub16(mhz19b.value, buff, 4));
					FSM_PCF8574_GoToXY(3,12);
					if(mhz19b._error) {
						FSM_PCF8574_AddStringFromFlash(PSTR("!"));
					} else {
						FSM_PCF8574_AddStringFromFlash(LNG_SMB_SPACE);
					}
					// Draw big clock data
					uint8_t pos = FSM_LCD_AddCustomCharStringToXY(utoa_cycle_sub8(rtc.hour, buff, 1, 2), 0, 0);
					FSM_LCD_AddCustomCharStringToXY(utoa_cycle_sub8(rtc.min, buff, 0, 2), 0, ++pos);
					// Draw blinking colon
					FSM_PCF8574_GoToXY(1,6);
					if((_colon_blink ^= 0x1)) {
						FSM_PCF8574_AddStringFromFlash(LNG_SMB_COLON);
					} else {
						FSM_PCF8574_AddStringFromFlash(LNG_SMB_SPACE);
					}
					// /@TODO: Need optimization
					break;
				}

				case DEVICE_MODE_SHOW_MENU: {
					if(device.flags._menu_changed) {
						// Flush menu change flag
						device.flags._menu_changed = 0;
						// Clear display
						FSM_PCF8574_Clear();
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
						// Draw menu on the LCD line by line
						for(uint8_t i=0; i < PCF8574_ROWS; i++) {
							// Draw cursor
							FSM_PCF8574_GoToXY(i,0);
							if(device.menu_cursor == i) {
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_ANGLE_BRACKET_RIGHT);
							} else {
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_SPACE);
							}
							// Draw menu item title
							uint8_t shift = i-device.menu_cursor;
							FSM_PCF8574_GoToXY(i,1);
							FSM_PCF8574_AddStringFromFlash(MENU_GetMenuTextByShift(shift));
							// Draw subcategory symbol
							FSM_PCF8574_GoToXY(i,PCF8574_COLS-2);
							FSM_PCF8574_AddStringFromFlash(MENU_GetChildMenuSymbolByShift(shift));
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
						FSM_PCF8574_GoToXY(0,0);
						FSM_PCF8574_AddStringFromFlash(LNG_IF_SETUP_DATE);
						FSM_PCF8574_GoToXY(PCF8574_ROWS-1, PCF8574_COLS-6);
						FSM_PCF8574_AddStringFromFlash(LNG_YES);
						FSM_PCF8574_GoToXY(PCF8574_ROWS-1, PCF8574_COLS-2);
						FSM_PCF8574_AddStringFromFlash(LNG_NO);

						// Draw cursor position
						switch(device.idx_curr) {

							case 1: {
								FSM_PCF8574_GoToXY(1,4);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_DBL_UNDERSCORE);
								break;
							}

							case 2: {
								FSM_PCF8574_GoToXY(1,7);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_DBL_UNDERSCORE);
								break;
							}

							case 3: {
								FSM_PCF8574_GoToXY(1,10);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_DBL_UNDERSCORE);
								break;
							}

							case 4: {
								FSM_PCF8574_GoToXY(PCF8574_ROWS-1, PCF8574_COLS-7);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_ANGLE_BRACKET_RIGHT);
								break;
							}

							case 5: {
								FSM_PCF8574_GoToXY(PCF8574_ROWS-1, PCF8574_COLS-3);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_ANGLE_BRACKET_RIGHT);
								break;
							}

							default: {
								FSM_PCF8574_GoToXY(1,1);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_DBL_UNDERSCORE);
								break;
							}
						}
					}
					// Draw DATE data
					FSM_PCF8574_GoToXY(2,0);
					FSM_PCF8574_AddStringFromFlash((char *)LNG_DOW[rtc.dow]);
					FSM_PCF8574_GoToXY(2,4);
					FSM_PCF8574_AddString(utoa_cycle_sub8(rtc.day, buff, 0, 2));
					FSM_PCF8574_AddStringFromFlash(LNG_SMB_SLASH);
					FSM_PCF8574_AddString(utoa_cycle_sub8(rtc.month, buff, 0, 2));
					FSM_PCF8574_AddStringFromFlash(LNG_SMB_SLASH);
					FSM_PCF8574_AddString(utoa_cycle_sub8(rtc.year, buff, 0, 2));
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
						FSM_PCF8574_GoToXY(0,0);
						FSM_PCF8574_AddStringFromFlash(LNG_IF_SETUP_TIME);
						FSM_PCF8574_GoToXY(PCF8574_ROWS-1, PCF8574_COLS-6);
						FSM_PCF8574_AddStringFromFlash(LNG_YES);
						FSM_PCF8574_GoToXY(PCF8574_ROWS-1, PCF8574_COLS-2);
						FSM_PCF8574_AddStringFromFlash(LNG_NO);

						// Draw cursor position
						switch(device.idx_curr) {

							case 1: {
								FSM_PCF8574_GoToXY(1,3);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_DBL_UNDERSCORE);
								break;
							}

							case 2: {
								FSM_PCF8574_GoToXY(1,6);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_DBL_UNDERSCORE);
								break;
							}

							case 3: {
								FSM_PCF8574_GoToXY(PCF8574_ROWS-1, PCF8574_COLS-7);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_ANGLE_BRACKET_RIGHT);
								break;
							}

							case 4: {
								FSM_PCF8574_GoToXY(PCF8574_ROWS-1, PCF8574_COLS-3);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_ANGLE_BRACKET_RIGHT);
								break;
							}

							default: {
								FSM_PCF8574_GoToXY(1,0);
								FSM_PCF8574_AddStringFromFlash(LNG_SMB_DBL_UNDERSCORE);
								break;
							}
						}
					}
					// Draw TIME data
					FSM_PCF8574_GoToXY(2,0);
					FSM_PCF8574_AddString(utoa_cycle_sub8(rtc.hour, buff, 0, 2));
					FSM_PCF8574_AddStringFromFlash(LNG_SMB_COLON);
					FSM_PCF8574_AddString(utoa_cycle_sub8(rtc.min, buff, 0, 2));
					FSM_PCF8574_AddStringFromFlash(LNG_SMB_COLON);
					FSM_PCF8574_AddString(utoa_cycle_sub8(rtc.sec, buff, 0, 2));
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
