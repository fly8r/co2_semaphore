/*
 * button.c
 *
 * Created: 31.01.2019 14:24:22
 *  Author: fly8r
 */
#include "include/button.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;
static					button_data_t	button;
/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_BUTTON_Init(void)
{
	// Set default state for FSM
	FSM_state = BUTTON_STATE_UP;
	// Flush current and last events
	button.event = button.last_event = BUTTON_EVENT_NOT_PRESSED;
}

void FSM_BUTTON_Process(void)
{
	switch (FSM_state) {

		case BUTTON_STATE_UP: {
			// If button pressed
			if(BTN_PRESSED) {
				// Set current state
				button.event = BUTTON_EVENT_NOT_PRESSED;
				// Flush long press counter
				button.lp_counter=0;
				// Set next FSM state
				FSM_state = BUTTON_STATE_DN;
				// Reset timer for next button scan
				ResetTimer(TIMER_BUTTON);
				} else if(button.event != BUTTON_EVENT_NOT_PRESSED) {
				// Store last button event
				button.last_event = button.event;
				// If we have event from button - send message
				SendMessageWParam(MSG_BTN_KEY_PRESSED, &button.last_event);
				// Flush current event
				button.event = BUTTON_EVENT_NOT_PRESSED;
			}
			return;
		}

		case BUTTON_STATE_DN: {
			// Debounce delay check
			if(GetTimer(TIMER_BUTTON) >= BUTTON_DEBOUNCE_DELAY) {
				// If key also pressed - processing long press
				if(BTN_PRESSED) {
					// Set button event to SHORT PRESS
					button.event = BUTTON_EVENT_SHORT_PRESS;
					// Long press counter check
					if(button.lp_counter++ >= BUTTON_LONG_PRESS_COUNT) {
						// Set next FSM state
						FSM_state = BUTTON_STATE_AL;
					}
					} else {
					// Set next FSM state
					FSM_state = BUTTON_STATE_UP;
				}
				// Flush timer for next debounce cycle
				ResetTimer(TIMER_BUTTON);
			}
			return;
		}

		case BUTTON_STATE_AL: {
			if(!BTN_PRESSED) {
				// Set current button event
				button.event = BUTTON_EVENT_LONG_PRESS;
				// Set next FSM state
				FSM_state = BUTTON_STATE_UP;
			}
			return;
		}

		default: break;
	}
}
