/*
 * swtimers.h
 *
 * Created: 23.01.2019 10:47:06
 *  Author: fly8r
 */
#ifndef SWTIMERS_H
#define SWTIMERS_H

#include <stdio.h>
#include <util/atomic.h>
#include "../../HAL.h"

//#define USE_GLOBAL_TIMERS

#define		SEC 				1000
#define		MIN 				60 * SEC
#define		HOUR 				60 * MIN

// Mas timers counter
#define		MAX_TIMERS			12
// Timers enum
enum TIMERS_ENUM
{
	TIMER_SYSTEM=0,
	TIMER_PCF8574,
	TIMER_LCD,
	TIMER_LCD_BL,
	TIMER_X1226,
	TIMER_DHT,
	TIMER_MHZ19B,
	TIMER_ENC,
	TIMER_BUTTON,
	TIMER_LEDS,
	TIMER_LEDS_BLINK,
	TIMER_BUZZER,
};

void		InitSystemTimer(void);
void		InitTimers(void);
void		ProcessTimers(uint8_t * tick);
uint16_t	GetTimer(uint8_t Timer);
void		ResetTimer(uint8_t Timer);

#ifdef USE_GLOBAL_TIMERS
#define MAX_GTIMERS 	1

enum GTIMERS_ENUM
{
	GTIMER_1=0,
};

uint16_t	GetGTimer(uint8_t Timer);
void		StopGTimer(uint8_t Timer);
void		StartGTimer(uint8_t Timer);
void		PauseGTimer(uint8_t Timer);
void		ResumeGTimer(uint8_t Timer);
#endif


#endif /* SWTIMERS_H_ */
