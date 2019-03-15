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
//    |- Clock setup >
//       |- Up
//       |- Time setup
//       |- Date setup
//    |- Buzzer setup
//    |- Backlight setup



//					NEXT,		PREVIOUS		PARENT,			CHILD
MAKE_MENU(m_s1i1,	m_s1i2,		NULL_ENTRY,		NULL_ENTRY,		NULL_ENTRY,		MENU_ACTION_CANCEL,	LNG_MENU_ITEM_MAIN_SCREEN, 1, 0);
//					NEXT,		PREVIOUS		PARENT,			CHILD
MAKE_MENU(m_s1i2,	m_s1i3,		m_s1i1,			NULL_ENTRY,		m_s2i1,			0,					LNG_MENU_ITEM_SETTINGS_CLOCK, 0, 1);
	//					NEXT,		PREVIOUS		PARENT,			CHILD
	MAKE_MENU(m_s2i1,	m_s2i2,		NULL_ENTRY,		m_s1i2,			m_s1i1,			0,								LNG_MENU_ITEM_UP, 1, 0);
	MAKE_MENU(m_s2i2,	m_s2i3,		m_s2i1,			m_s1i2,			m_s2i1,			MENU_ACTION_DATETIME_SET_TIME,	LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_SETUP, 0, 0);
	MAKE_MENU(m_s2i3,	NULL_ENTRY, m_s2i2,			m_s1i2,			m_s2i1,			MENU_ACTION_DATETIME_SET_DATE,	LNG_MENU_ITEM_SETTINGS_CLOCK_TIME_DATE, 0, 0);
//					NEXT,		PREVIOUS		PARENT,			CHILD
MAKE_MENU(m_s1i3,	m_s1i4,		m_s1i2,			NULL_ENTRY,		m_s1i1,			MENU_ACTION_BUZZER_SET,		LNG_MENU_ITEM_SETTINGS_ALARM_BUZZER, 0, 0);
//					NEXT,		PREVIOUS		PARENT,			CHILD
MAKE_MENU(m_s1i4,	NULL_ENTRY,	m_s1i3,			NULL_ENTRY,		m_s1i1,			MENU_ACTION_LCD_BL_SET,		LNG_MENU_ITEM_SETTINGS_LCD_BACKLIGHT, 0, 0);

void MENU_Change(menu_item_t *new_menu)
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

	if ((void*)temp_menu_item != (void*)&NULL_ENTRY) {
		if(pgm_read_byte(&temp_menu_item->_icon_top)) {
			return (char *)PSTR("\x2");
		}
		if(pgm_read_byte(&temp_menu_item->_icon_child)) {
			return (char *)LNG_SMB_ANGLE_BRACKET_RIGHT;
		}
	}
	return (char *)strNULL;
}