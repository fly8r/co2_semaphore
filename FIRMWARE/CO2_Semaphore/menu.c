/*
 * menu.c
 *
 * Created: 04.02.2019 15:16:16
 *  Author: fly8r
 */
#include "menu.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
const		char	strNULL[]	PROGMEM = "";


/************************************************************************/
/* Menu structure                                                       */
/************************************************************************/
// Menu
// |- Main screen
// |- Settings >
//    |- Up >
//    |- Clock >
//       |- Up
//       |- Time setup
//       |- Date setup
//       |- Calibration
//    |- Alarm >
//       |- Up
//       |- Levels setup
//       |- Buzzer setup
//    |- LCD Backlight
// |- Monitoring >
//    |- Up
//    |- CO2
//    |- Temperature
//    |- Humidity



//					NEXT,		PREVIOUS		PARENT,			CHILD
MAKE_MENU(m_s1i1,	m_s1i2,		NULL_ENTRY,		NULL_ENTRY,		NULL_ENTRY,	MENU_ACTION_CANCEL,	LNG_MENU_ITEM_MAIN_SCREEN);
MAKE_MENU(m_s1i2,	m_s1i3,		m_s1i1,			NULL_ENTRY,		m_s2i1,		0,					LNG_MENU_ITEM_SETTINGS);
	//					NEXT,		PREVIOUS		PARENT,			CHILD
	MAKE_MENU(m_s2i1,	m_s2i2,		NULL_ENTRY,		m_s1i2,			m_s1i1,			0,	LNG_MENU_ITEM_UP);
	MAKE_MENU(m_s2i2,	m_s2i3,		m_s2i1,			m_s1i2,			m_s3i1,			0,	LNG_MENU_ITEM_SETTINGS_CLOCK);
		//					NEXT,		PREVIOUS		PARENT,			CHILD
		MAKE_MENU(m_s3i1,	m_s3i2,		NULL_ENTRY,		m_s2i2,			m_s2i1,			0,								LNG_MENU_ITEM_UP);
		MAKE_MENU(m_s3i2,	m_s3i3,		m_s3i1,			m_s2i2,			m_s3i1,			MENU_ACTION_DATETIME_SET_TIME,	LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_SETUP);
		MAKE_MENU(m_s3i3,	m_s3i4,		m_s3i2,			m_s2i2,			m_s3i1,			MENU_ACTION_DATETIME_SET_DATE,	LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_DATE);
		MAKE_MENU(m_s3i4,	NULL_ENTRY,	m_s3i3,			m_s2i2,			m_s3i1,			0,								LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_CAL);
	//					NEXT,		PREVIOUS		PARENT,			CHILD
	MAKE_MENU(m_s2i3,	m_s2i4,		m_s2i2,			m_s1i2,			m_s4i1,			0,	LNG_MENU_ITEM_SETTINGS_ALARM);
		//					NEXT,		PREVIOUS		PARENT,			CHILD
		MAKE_MENU(m_s4i1,	m_s4i2,		NULL_ENTRY,	m_s2i3,			m_s2i1,			0,	LNG_MENU_ITEM_UP);
		MAKE_MENU(m_s4i2,	m_s4i3,		m_s4i1,		m_s2i3,			NULL_ENTRY,		0,	LNG_MENU_ITEM_SETTINGS_ALARM_LEVELS);
		MAKE_MENU(m_s4i3,	NULL_ENTRY,	m_s4i2,		m_s2i3,			NULL_ENTRY,		0,	LNG_MENU_ITEM_SETTINGS_ALARM_BUZZER);
	//					NEXT,		PREVIOUS		PARENT,			CHILD
	MAKE_MENU(m_s2i4,	NULL_ENTRY,	m_s2i3,			m_s1i2,			NULL_ENTRY,		0,	LNG_MENU_ITEM_SETTINGS_LCD_BACKLIGHT);
//					NEXT,		PREVIOUS		PARENT,			CHILD
MAKE_MENU(m_s1i3,	NULL_ENTRY,	m_s1i2,			NULL_ENTRY,		m_s5i1,		0,					LNG_MENU_ITEM_MONITORING);
	//					NEXT,		PREVIOUS		PARENT,			CHILD
	MAKE_MENU(m_s5i1,	m_s5i2,		NULL_ENTRY,		m_s1i3,			m_s1i1,			0,	LNG_MENU_ITEM_UP);
	MAKE_MENU(m_s5i2,	m_s5i3,		m_s5i1,			m_s1i3,			NULL_ENTRY,		0,	LNG_MENU_ITEM_MONITORING_CO2);
	MAKE_MENU(m_s5i3,	m_s5i4,		m_s5i2,			m_s1i3,			NULL_ENTRY, 	0,	LNG_MENU_ITEM_MONITORING_T);
	MAKE_MENU(m_s5i4,	NULL_ENTRY,	m_s5i3,			m_s1i3,			NULL_ENTRY,		0,	LNG_MENU_ITEM_MONITORING_H);



void MENU_Change(menu_item_t* new_menu)
{
	if((void*)new_menu == (void*)&NULL_ENTRY)
		return;
	selected_menu_item = new_menu;
	device.flags._menu_changed = 1;
}

void MENU_Start(void)
{
	selected_menu_item = (menu_item_t *)&m_s1i1;
	device.flags._menu_changed = 1;
}

char *MENU_GetMenuTextByShift(int8_t shift)
{
	int8_t i;
	menu_item_t *temp_menu_item;

	if ((void*)selected_menu_item == (void*)&NULL_ENTRY)
		return (char *)strNULL;

	i = shift;
	temp_menu_item = selected_menu_item;
	if (i>0) {
		while( i!=0 ) {
			if ((void*)temp_menu_item != (void*)&NULL_ENTRY) {
				temp_menu_item = (menu_item_t *)pgm_read_word(&temp_menu_item->Next);
			}
			i--;
		}
	} else {
		while( i!=0 ) {
			if ((void*)temp_menu_item != (void*)&NULL_ENTRY) {
				temp_menu_item = (menu_item_t *)pgm_read_word(&temp_menu_item->Previous);
			}
			i++;
		}
	}

	if ((void*)temp_menu_item == (void*)&NULL_ENTRY) {
		return (char *)strNULL;
	} else {
		return ((char *)pgm_read_word(&temp_menu_item->Text));
	}
}

char *MENU_GetChildMenuSymbolByShift(int8_t shift)
{
	int8_t i;
	menu_item_t *temp_menu_item;

	if ((void*)selected_menu_item == (void*)&NULL_ENTRY)
		return (char *)strNULL;

	i = shift;
	temp_menu_item = selected_menu_item;
	if (i>0) {
		while( i!=0 ) {
			if ((void*)temp_menu_item != (void*)&NULL_ENTRY) {
				temp_menu_item = (menu_item_t *)pgm_read_word(&temp_menu_item->Next);
			}
			i--;
		}
	} else {
		while( i!=0 ) {
			if ((void*)temp_menu_item != (void*)&NULL_ENTRY) {
				temp_menu_item = (menu_item_t *)pgm_read_word(&temp_menu_item->Previous);
			}
			i++;
		}
	}

	if ((void*)temp_menu_item == (void*)&NULL_ENTRY) {
		return (char *)strNULL;
	} else {
		if((char *)pgm_read_word(&temp_menu_item->Child) != (void *)&NULL_ENTRY) {
			return (char *)LNG_SMB_ANGLE_BRACKET_RIGHT;
		} else {
			return (char *)strNULL;
		}
	}
}