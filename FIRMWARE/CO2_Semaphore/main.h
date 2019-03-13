/*
 * main.h
 *
 * Created: 23.01.2019 10:32:35
 *  Author: fly8r
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "global_vars.h"
#include "config.h"
#include "HAL.h"
#include "libs/include/messages.h"
#include "libs/include/swtimers.h"
#include "drivers/include/i2c.h"
#include "drivers/include/uart.h"
#include "fsms/include/system.h"
#include "fsms/include/uart.h"
#include "fsms/include/pcf8574.h"
#include "fsms/include/lcd.h"
#include "fsms/include/lcd_bl_ctrl.h"
#include "fsms/include/x1226.h"
#include "fsms/include/dht.h"
#include "fsms/include/mhz19b.h"
#include "fsms/include/button.h"
#include "fsms/include/encoder.h"
#include "fsms/include/leds.h"
#include "fsms/include/buzzer.h"
#include "fsms/include/charts.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
static		uint8_t		systick;		// System tick timer
