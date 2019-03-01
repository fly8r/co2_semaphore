/*
 * gloval_vars.c
 *
 * Created: 23.01.2019 10:41:03
 *  Author: fly8r
 */
#include "global_vars.h"


/************************************************************************/
/* main.h                                                               */
/************************************************************************/
device_data_t	device;
clock_data_t	rtc;

/************************************************************************/
/* system.h                                                             */
/************************************************************************/
const			uint8_t			bl_pwm_table[]	PROGMEM = { 0, 2, 5, 10, 20, 30, 55, 90, 115, 200, 255 };

/************************************************************************/
/* menu.h                                                               */
/************************************************************************/
menu_item_t		*selected_menu_item;
const			menu_item_t		Null_Menu = {(void *)0, (void *)0, (void *)0, (void *)0, 0, 0};

/************************************************************************/
/* mhz19b.h                                                             */
/************************************************************************/
mhz19b_data_t	mhz19b;

/************************************************************************/
/* dht.h                                                                */
/************************************************************************/
dht_data_t		dht;
