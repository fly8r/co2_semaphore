/*
 * charts.c
 *
 * Created: 10.03.2019 21:32:04
 *  Author: fly8r
 */
#include "include/charts.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;
static					uint8_t			minute_counter, hour_counter;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_CHARTS_Init(void)
{
	FSM_state = FSM_CHARTS_STATE_IDLE;
	ResetTimer(TIMER_CHARTS);
}

void FSM_CHARTS_Process(void)
{
	switch(FSM_state) {

		case FSM_CHARTS_STATE_COUNTERS_PROCESSING: {
			// Increment time counters
			minute_counter++;
			hour_counter++;
			/* Minute counter complete */
			if(minute_counter >= CHART_BY_MINUTE_STORE_PERIOD) {
				// Flush counter
				minute_counter = 0;
				// Shift values in buffer for history organization
				for(int8_t i=13; i>=0; i--) {
					device.charts.co2.by_minute[i+1] = device.charts.co2.by_minute[i];
					device.charts.h.by_minute[i+1] = device.charts.h.by_minute[i];
					device.charts.t.by_minute[i+1] = device.charts.t.by_minute[i];
				}
				// Store values to buffer
				device.charts.co2.by_minute[0] = mhz19b.value;
				device.charts.h.by_minute[0] = dht.humidity.value;
				if(!dht.temperature.sign) {
					device.charts.t.by_minute[0] = dht.temperature.value;
				} else {
					device.charts.t.by_minute[0] = 0;
				}
			}
			/* Hour counter complete */
			if(hour_counter >= CHART_BY_HOUR_STORE_PERIOD) {
				// Flush counter
				hour_counter = 0;
				// Shift values in buffer for history organization
				for(uint8_t i=13; i>=0; i--) {
					device.charts.co2.by_hour[i+1] = device.charts.co2.by_hour[i];
					device.charts.h.by_hour[i+1] = device.charts.h.by_hour[i];
					device.charts.t.by_hour[i+1] = device.charts.t.by_hour[i];
				}
				// Calculate hourly average CO2
				uint16_t t = 0;
				for(uint8_t i=0; i<15; i++) {
					t += device.charts.co2.by_minute[i];
				}
				device.charts.co2.by_hour[0] = t / 15;
				// Calculate hourly average Humidity
				t=0;
				for(uint8_t i=0; i<15; i++) {
					t += device.charts.h.by_minute[i];
				}
				device.charts.h.by_hour[0] = t / 15;
				// Calculate hourly average Temperature
				t=0;
				for(uint8_t i=0; i<15; i++) {
					t += device.charts.t.by_minute[i];
				}
				device.charts.t.by_hour[0] = t / 15;
			}
			// Goto default state
			FSM_state = FSM_CHARTS_STATE_IDLE;
			return;
		}

		case FSM_CHARTS_STATE_IDLE: {
			// Processing counters every 60 seconds
			if(GetTimer(TIMER_CHARTS) >= 60000) {
				// Flush timer
				ResetTimer(TIMER_CHARTS);
				// Goto processing state
				FSM_state = FSM_CHARTS_STATE_COUNTERS_PROCESSING;
			}
			return;
		}

		default: break;
	}

}