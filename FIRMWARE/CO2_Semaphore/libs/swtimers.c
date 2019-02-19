/*
 * swtimers.c
 *
 * Created: 23.01.2019 10:48:17
 *  Author: fly8r
 */
#include "include/swtimers.h"


/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint16_t	Timers[MAX_TIMERS];

#ifdef USE_GLOBAL_TIMERS
#define TIMER_STOPPED		0
#define TIMER_RUNNING		1
#define TIMER_PAUSED		2

volatile	static	uint16_t	GTimers[MAX_GTIMERS];
volatile	static	uint8_t		GTStates[MAX_GTIMERS];
#endif

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void InitTimers(void)
{
	for(uint8_t i=0;i<MAX_TIMERS;i++) {
		Timers[i] = 0;
	}
	#ifdef USE_GLOBAL_TIMERS
	for(uint8_t i=0;i<MAX_GTIMERS;i++) {
		GTimers[i] = 0;
		GTStates[i] = TIMER_STOPPED;
	}
	#endif
}

void ProcessTimers(uint8_t * tick) {
	uint8_t x = *tick;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (x > 0) {
			for (uint8_t i=0; i<MAX_TIMERS; i++) {
				Timers[i] += x;

				#ifdef USE_GLOBAL_TIMERS
				if (GTStates[i] == TIMER_RUNNING) {
					GTimers[i] += (uint16_t)x;
				}
				#endif
			}
			*tick = 0;
		}
	}
}

uint16_t GetTimer(uint8_t Timer) {
	return Timers[Timer];
}

void ResetTimer(uint8_t Timer) {
	Timers[Timer] = 0;
}

#ifdef USE_GLOBAL_TIMERS
uint16_t  GetGTimer(uint8_t Timer){
	return GTimers[Timer];
}

void StopGTimer(uint8_t Timer){
	GTStates[Timer] = TIMER_STOPPED;
}
void StartGTimer(uint8_t Timer){
	if (GTStates[Timer] == TIMER_STOPPED) {
		GTimers[Timer] = 0;
		GTStates[Timer] = TIMER_RUNNING;
	}
}
void PauseGTimer(uint8_t Timer) {
	if (GTStates[Timer] == TIMER_RUNNING) {
		GTStates[Timer] = TIMER_PAUSED;
	}
}
void ContinueGTimer(uint8_t Timer) {
	if (GTStates[Timer] == TIMER_PAUSED) {
		GTStates[Timer] = TIMER_RUNNING;
	}
}
#endif
