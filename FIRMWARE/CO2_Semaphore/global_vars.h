/*
 * global_vars.h
 *
 * Created: 23.01.2019 10:40:49
 *  Author: fly8r
 */
#ifndef GLOBAL_VARS_H_
#define GLOBAL_VARS_H_

#include <stdio.h>
#include <avr/pgmspace.h>
#include "config.h"

/************************************************************************/
/* Macros                                                               */
/************************************************************************/
#define		bit(bn)	(1<<bn)


/************************************************************************/
/* main.h                                                               */
/************************************************************************/
enum DEVICE_MODES_ENUM
{
	DEVICE_MODE_IDLE=1,
	DEVICE_MODE_SHOW_MENU,
	DEVICE_MODE_DATETIME_SET_DATE,
	DEVICE_MODE_DATETIME_SET_TIME,
	DEVICE_MODE_LCD_BL_SET,
};

enum DEVICE_CONCENTRATION_ENUM
{
	DEVICE_CONCENTRATION_UNKNOWN=0,
	DEVICE_CONCENTRATION_NORMAL,
	DEVICE_CONCENTRATION_NORMAL_ABOVE,
	DEVICE_CONCENTRATION_MIDDLE,
	DEVICE_CONCENTRATION_MIDDLE_ABOVE,
	DEVICE_CONCENTRATION_HIGH
};

typedef struct
{
	uint8_t							_ext_clock;
	uint8_t							idx_curr, menu_cursor;
	enum DEVICE_MODES_ENUM			mode;
	enum DEVICE_MODES_ENUM			last_mode;
	enum DEVICE_CONCENTRATION_ENUM	concentration_level, last_concentration_level;
	struct {
		uint8_t		_menu_changed,
					_idx_changed;
	} flags;
} device_data_t;
extern	device_data_t	device;

typedef struct
{
	uint8_t		_presence;
	uint8_t		sec,
				min,
				hour,
				dow,
				day,
				month,
				year;
	int8_t		cal;
} clock_data_t;
extern		clock_data_t	rtc;

/************************************************************************/
/* menu.h                                                               */
/************************************************************************/
enum MENU_ACTIONS_ENUM
{
	MENU_ACTION_CANCEL = 1,
	MENU_ACTION_DATETIME_SET_DATE,
	MENU_ACTION_DATETIME_SET_TIME,
	MENU_ACTION_DATETIME_SET_CAL,
	MENU_ACTION_LCD_BL_SET,
};


typedef struct PROGMEM {
	void			*Next;
	void			*Previous;
	void			*Parent;
	void			*Child;
	uint8_t			Select;
	const char		*Text;
} menu_item_t;
extern			menu_item_t		*selected_menu_item;
extern	const	menu_item_t		Null_Menu;

#define NULL_ENTRY		Null_Menu
#define MENU_PREVIOUS   ((menu_item_t *)pgm_read_word(&selected_menu_item->Previous))
#define MENU_NEXT       ((menu_item_t *)pgm_read_word(&selected_menu_item->Next))
#define MENU_PARENT     ((menu_item_t *)pgm_read_word(&selected_menu_item->Parent))
#define MENU_CHILD      ((menu_item_t *)pgm_read_word(&selected_menu_item->Child))
#define MENU_SELECT		(pgm_read_byte(&selected_menu_item->Select))

#define MAKE_MENU(Name, Next, Previous, Parent, Child, Select, Text) \
extern const menu_item_t Next;     \
extern const menu_item_t Previous; \
extern const menu_item_t Parent;   \
extern const menu_item_t Child;  \
const menu_item_t Name = {(void *)&Next, (void *)&Previous, (void *)&Parent, (void *)&Child, (uint8_t)Select, Text}

/************************************************************************/
/* mhz19b.h                                                             */
/************************************************************************/
enum	MHZ19B_ERRORS_ENUM
{
	MHZ19B_ERROR_NO_ERROR,
	MHZ19B_ERROR_NO_RESPONSE,
	MHZ19B_ERROR_DATA_CORRUPT,
	MHZ19B_ERROR_VALUE_TOO_BIG,
	MHZ19B_ERROR_CRC,
};

typedef struct
{
	uint8_t		_presence, _error;
	enum		MHZ19B_ERRORS_ENUM	error;
	uint16_t	value;
} mhz19b_data_t;
extern	mhz19b_data_t	mhz19b;

/************************************************************************/
/* dht.h                                                                */
/************************************************************************/
enum	DHT_ERRORS_ENUM
{
	DHT_ERROR_NO_ERROR,
	DHT_ERROR_NO_DEVICE,
	DHT_ERROR_READY_TIMEOUT,
	DHT_ERROR_TIMEOUT,
	DHT_ERROR_CRC
};

typedef struct
{
	uint8_t		_failed, _error;
	enum		DHT_ERRORS_ENUM		error;
	struct
	{
		uint16_t	h, t;
		uint8_t		crc;
	} data;
	struct
	{
		uint8_t	sign, value, mantissa;
	} temperature;
	struct
	{
		uint8_t	value, mantissa;
	} humidity;
} dht_data_t;
extern		dht_data_t		dht;


/************************************************************************/
/* button.h                                                             */
/************************************************************************/
enum BUTTON_EVENTS_ENUM
{
	BUTTON_EVENT_NOT_PRESSED,   // No events
	BUTTON_EVENT_SHORT_PRESS,   // Short press event
	BUTTON_EVENT_LONG_PRESS     // Long press event
};


/************************************************************************/
/* leds.h                                                               */
/************************************************************************/
enum LED_COLORS_ENUM
{
	RED=0,
	YELLOW,
	GREEN,
	NONE
};

typedef struct
{
	uint8_t		_enable, _active, _blinking;
	enum		LED_COLORS_ENUM		color, color2;
	uint16_t	glow_time_ms;
} leds_params_t;


/************************************************************************/
/* buzzer.h                                                             */
/************************************************************************/
enum BUZZER_PULSE_MODES_ENUM
{
	BUZZER_PULSE_MODE_INTERVAL_EQUAL,
	BUZZER_PULSE_MODE_INTERVAL_NOT_EQUAL,
};

typedef struct
{
	uint8_t								_enable, _active, _curr_state;
	enum	BUZZER_PULSE_MODES_ENUM		pulse_mode;
	uint16_t							pulse_count;
} buzzer_data_t;



#endif /* GLOBAL_VARS_H_ */
