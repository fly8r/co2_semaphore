/*
 * strings.h
 *
 * Created: 23.01.2019 15:01:37
 *  Author: fly8r
 */
#ifndef STRINGS_H_
#define STRINGS_H_

#include <stdio.h>
#include <avr/pgmspace.h>


extern	const	char
		icon_temp[8],
		icon_degree[8],
		icon_humidity[8],
		icon_underscore[8],
		icon_arrow_up[8],
		icon_arrow_down[8],
		icon_arrow_right[8],
		icon_colon[8];

extern	const	char
		LNG_SPLASH_LOGO_S1[],
		LNG_SPLASH_LOGO_S1_1[],
		LNG_SPLASH_LOGO_S4[];

extern  const	char
		LNG_HW_VERSION[],
		LNG_SW_VERSION[];

extern	const	char
		LNG_DM_SCHK[],
		LNG_DM_MAIN[],
		LNG_DM_SETUP_DATE[],
		LNG_DM_SETUP_TIME[],
		LNG_DM_SETUP_LCD_BL[];

extern	const	char
		LNG_SMB_ANGLE_BRACKET_RIGHT[],
		LNG_SMB_COLON[],
		LNG_SMB_PERCENT[],
		LNG_SMB_PLUS[],
		LNG_SMB_POINT[],
		LNG_SMB_MINUS[],
		LNG_SMB_SPACE[],
		LNG_SMB_SLASH[],
		LNG_SMB_DBL_UNDERSCORE[];

extern	const	char
		LNG_FAIL[],
		LNG_OK[],
		LNG_YES[],
		LNG_NO[];

extern	const	char
		LNG_MENU_ITEM_SETTINGS[],
		LNG_MENU_ITEM_SETTINGS_CLOCK[],
		LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_SETUP[],
		LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_DATE[],
		LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_CAL[],
		LNG_MENU_ITEM_SETTINGS_ALARM[],
		LNG_MENU_ITEM_SETTINGS_ALARM_LEVELS[],
		LNG_MENU_ITEM_SETTINGS_ALARM_BUZZER[],
		LNG_MENU_ITEM_SETTINGS_LCD_BACKLIGHT[],
		LNG_MENU_ITEM_MONITORING[],
		LNG_MENU_ITEM_MONITORING_CO2[],
		LNG_MENU_ITEM_MONITORING_T[],
		LNG_MENU_ITEM_MONITORING_H[],
		LNG_MENU_ITEM_MAIN_SCREEN[],
		LNG_MENU_ITEM_UP[];

extern	const	char
		LNG_MONDAY[],
		LNG_TUESDAY[],
		LNG_WEDNESDAY[],
		LNG_THURSDAY[],
		LNG_FRIDAY[],
		LNG_SATURDAY[],
		LNG_SUNDAY[];

extern	const	char	*LNG_DOW[];

#endif