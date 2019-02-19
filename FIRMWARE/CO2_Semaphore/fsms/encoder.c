/*
 * encoder.c
 *
 * Created: 06.12.2018 11:02:43
 *  Author: fly8r
 */
#include "include/encoder.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;
static					encoder_data_t	encoder;
/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_ENC_Init(void)
{
	// Set default FSM state
	FSM_state = FSM_ENC_STATE_IDLE;
	// Flush FSM timer
	ResetTimer(TIMER_ENC);
}

void FSM_ENC_Process(void)
{
	switch(FSM_state) {

		case FSM_ENC_STATE_IDLE: {
			if(GetTimer(TIMER_ENC) >= 1) {
				// Set next FSM state
				FSM_state = FSM_ENC_STATE_SCAN;
				// Flush FSM timer
				ResetTimer(TIMER_ENC);
				return;
			}
			return;
		}

		case FSM_ENC_STATE_SCAN: {
			// Getting current encoder pin state
			uint8_t curr_state = ENC_STATE;
			// Processing state
			switch(encoder.prev_state) {
				case 0: {
					if(curr_state == 2) encoder.value--;
					if(curr_state == 1) encoder.value++;
					break;
				}

				case 1: {
					if(curr_state == 0) encoder.value--;
					if(curr_state == 3) encoder.value++;
					break;
				}


				case 2: {
					if(curr_state == 3) encoder.value--;
					if(curr_state == 0) encoder.value++;
					break;
				}

				case 3: {
					if(curr_state == 1) encoder.value--;
					if(curr_state == 2) encoder.value++;
					break;
				}
			}
			// Save last state of encoder pin
			encoder.prev_state = curr_state;
			// If counter not null
			if(encoder.value > 3 || encoder.value < -3) {
				// Set next FSM state
				FSM_state = FSM_ENC_STATE_PROCESSING;
				return;
			}
			// Set next FSM state
			FSM_state = FSM_ENC_STATE_IDLE;
			return;
		}

		case FSM_ENC_STATE_PROCESSING: {
			// Store last rotate value
			encoder.last_value = encoder.value;
			// Flush encoder counter
			encoder.value = 0;
			// Send message with encoder rotate angel
			SendMessageWParam(MSG_ENC_ROTATE, &encoder.last_value);
			// Set next FSM state
			FSM_state = FSM_ENC_STATE_IDLE;
			return;
		}

		default: break;
	}
}
