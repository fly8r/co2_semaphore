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
		level0[8],
		level1[8],
		level2[8],
		level3[8],
		level4[8],
		level5[8],
		level6[8],
		level7[8];

extern	const	char
		cc_icon_enter[],
		cc_icon_solid_rarrow[],
		cc_icon_top[];

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
		LNG_DM_CHART_CO2_HOURLY[],
		LNG_DM_CHART_CO2_DAILY[],
		LNG_DM_CHART_H_HOURLY[],
		LNG_DM_CHART_H_DAILY[],
		LNG_DM_CHART_T_HOURLY[],
		LNG_DM_CHART_T_DAILY[],
		LNG_DM_SETUP_DATE[],
		LNG_DM_SETUP_TIME[],
		LNG_DM_SETUP_LCD_BL[],
		LNG_DM_SETUP_BUZZER[];

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
		LNG_NO[],
		LNG_ON[],
		LNG_OFF[];

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