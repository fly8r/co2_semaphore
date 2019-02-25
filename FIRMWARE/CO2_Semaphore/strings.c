/*
 * strings.c
 *
 * Created: 23.01.2019 15:01:48
 *  Author: fly8r
 */
#include "strings.h"

/************************************************************************/
/* Custom HD44780 chars                                                 */
/************************************************************************/
const	char	icon_temp[8]	PROGMEM = {
	0b00000100,			//     *
	0b00001010,			//    * *
	0b00001010,			//    * *
	0b00001110,			//    ***
	0b00011111,			//   *****
	0b00011111,			//   *****
	0b00001110,			//    ***
	0b00000000			//
};
const	char	icon_degree[8]	PROGMEM = {
	0b00011100,			//   ***
	0b00010100,			//   * *
	0b00011100,			//   ***
	0b00000000,			//
	0b00000000,			//
	0b00000000,			//
	0b00000000,			//
	0b00000000			//
};
const	char	icon_humidity[8]		PROGMEM = {
	0b00000100,			//     *
	0b00000100,			//     *
	0b00001110,			//    ***
	0b00001110,			//    ***
	0b00011111,			//   *****
	0b00011111,			//   *****
	0b00001110,			//    ***
	0b00000000			//
};
const	char	icon_arrow_up[8]		PROGMEM = {
	0b00000000,			//
	0b00000100,			//     *
	0b00001110,			//	  ***
	0b00011111,			//   *****
	0b00001110,			//    ***
	0b00001110,			//    ***
	0b00000000,			//
	0b00000000			//
};
const	char	icon_arrow_down[8]		PROGMEM = {
	0b00000000,			//
	0b00001110,			//    ***
	0b00001110,			//    ***
	0b00011111,			//   *****
	0b00001110,			//    ***
	0b00000100,			//     *
	0b00000000,			//
	0b00000000			//
};
const	char	icon_arrow_right[8]		PROGMEM = {
	0b00000000,			//
	0b00010000,			//   *
	0b00011000,			//   **
	0b00011100,			//   ***
	0b00011000,			//   **
	0b00010000,			//   *
	0b00000000,			//
	0b00000000			//
};
const	char	icon_underscore[8]		PROGMEM = {
	0b00000000,			//
	0b00000000,			//
	0b00000000,			//
	0b00000000,			//
	0b00000000,			//
	0b00000000,			//
	0b00000000,			//
	0b00011111			//	 *****
};
const	char	icon_colon[8]		PROGMEM = {
	0b00000000,			//
	0b00000000,			//
	0b00001100,			//    **
	0b00001100,			//    **
	0b00000000,			//
	0b00001100,			//    **
	0b00001100,			//    **
	0b00000000			//
};

const	char
			LNG_SPLASH_LOGO_S1[]						PROGMEM = "C0",
			LNG_SPLASH_LOGO_S1_1[]						PROGMEM = "2",
			LNG_SPLASH_LOGO_S4[]						PROGMEM = "SEMAPHORE";

const	char
			LNG_HW_VERSION[]							PROGMEM = "HW: 1.0",
			LNG_SW_VERSION[]							PROGMEM = "SW: 1.0";

const	char
			LNG_DM1_S1[]								PROGMEM = "Peripheral check:\r\n",
			LNG_DM1_S2[]								PROGMEM = " -> RTC ...\r\n",
			LNG_DM1_S3[]								PROGMEM = " -> DHT ...\r\n",
			LNG_DM1_S4[]								PROGMEM = " -> CO2 ...";

const	char
			LNG_DM_MAIN_S4[]							PROGMEM = "   \xDF""C    %       ppm",
			LNG_DM_SETUP_DATE[]							PROGMEM = "Setup date:\r\n\r\n\r\nSave?",
			LNG_DM_SETUP_TIME[]							PROGMEM = "Setup time:\r\n\r\n\r\nSave?",
			LNG_DM_SETUP_LCD_BL[]						PROGMEM = "LCD backlight:\r\n\r\nLevel ->\r\nSave?";


const	char
			LNG_SMB_ANGLE_BRACKET_RIGHT[]				PROGMEM = ">",
			LNG_SMB_COLON[]								PROGMEM = ":",
			LNG_SMB_PERCENT[]							PROGMEM = "%",
			LNG_SMB_PLUS[]								PROGMEM = "+",
			LNG_SMB_POINT[]								PROGMEM = ".",
			LNG_SMB_MINUS[]								PROGMEM	= "-",
			LNG_SMB_SPACE[]								PROGMEM = " ",
			LNG_SMB_SLASH[]								PROGMEM = "/",
			LNG_SMB_DBL_UNDERSCORE[]					PROGMEM = "__";

const	char
			LNG_FAIL[]									PROGMEM = "FAIL",
			LNG_OK[]									PROGMEM = "OK",
			LNG_YES[]									PROGMEM = "Yes",
			LNG_NO[]									PROGMEM = "No";


const	char
			LNG_MENU_ITEM_SETTINGS[]					PROGMEM = "Settings",
			LNG_MENU_ITEM_SETTINGS_CLOCK[]				PROGMEM = "Clock",
			LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_SETUP[]	PROGMEM = "Time setup",
			LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_DATE[]	PROGMEM = "Date setup",
			LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_CAL[]		PROGMEM = "Calibration",
			LNG_MENU_ITEM_SETTINGS_ALARM[]				PROGMEM = "Alarm",
			LNG_MENU_ITEM_SETTINGS_ALARM_LEVELS[]		PROGMEM = "Levels setup",
			LNG_MENU_ITEM_SETTINGS_ALARM_BUZZER[]		PROGMEM = "Buzzer setup",
			LNG_MENU_ITEM_SETTINGS_LCD_BACKLIGHT[]		PROGMEM = "LCD Backlight",
			LNG_MENU_ITEM_MONITORING[]					PROGMEM = "Monitoring",
			LNG_MENU_ITEM_MONITORING_CO2[]				PROGMEM = "CO2 Graph",
			LNG_MENU_ITEM_MONITORING_T[]				PROGMEM = "Temperature Graph",
			LNG_MENU_ITEM_MONITORING_H[]				PROGMEM = "Humidity Graph",
			LNG_MENU_ITEM_MAIN_SCREEN[]					PROGMEM = "Main screen",
			LNG_MENU_ITEM_UP[]							PROGMEM = "*Up";


const	char
			LNG_MONDAY[]								PROGMEM = "Mon",
			LNG_TUESDAY[]								PROGMEM = "Tue",
			LNG_WEDNESDAY[]								PROGMEM = "Wed",
			LNG_THURSDAY[]								PROGMEM = "Thu",
			LNG_FRIDAY[]								PROGMEM = "Fri",
			LNG_SATURDAY[]								PROGMEM = "Sat",
			LNG_SUNDAY[]								PROGMEM = "Sun";

const	char
			*LNG_DOW[] = { LNG_MONDAY, LNG_TUESDAY, LNG_WEDNESDAY, LNG_THURSDAY, LNG_FRIDAY, LNG_SATURDAY, LNG_SUNDAY };