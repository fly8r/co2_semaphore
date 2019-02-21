/*
 * buzzer.c
 *
 * Created: 19.02.2019 14:48:59
 *  Author: fly8r
 */
#include "include/buzzer.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;
static					buzzer_data_t	*buzzer_params;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/

void FSM_BUZZER_Init(void)
{
	FSM_state = FSM_BUZZER_STATE_IDLE;
}

void FSM_BUZZER_Process(void)
{
	uint16_t curr_period=0;

	switch(FSM_state) {

		case FSM_BUZZER_STATE_PGM_PROCESSING: {
			//
			if(!buzzer_params->pulse_count) {
				BUZZER_OFF();
				FSM_state = FSM_BUZZER_STATE_IDLE;
				return;
			}
			//
			switch(buzzer_params->pulse_mode) {
				case BUZZER_PULSE_MODE_INTERVAL_EQUAL: {
					curr_period = BUZZER_PULSE_EQUAL_PERIOD;
					break;
				}

				case BUZZER_PULSE_MODE_INTERVAL_NOT_EQUAL: {
					curr_period = BUZZER_PULSE_NOT_EQUL_ON_PERIOD;
					if(buzzer_params->_curr_state) {
						curr_period = BUZZER_PULSE_NOT_EQUL_OFF_PERIOD;
					}
					break;
				}
			}
			if(GetTimer(TIMER_BUZZER) >= curr_period) {
				if(buzzer_params->_curr_state) {
					BUZZER_OFF();
					buzzer_params->_curr_state = 0;
					buzzer_params->pulse_count--;
				} else {
					BUZZER_ON();
					buzzer_params->_curr_state = 1;
				}
				ResetTimer(TIMER_BUZZER);
			}
			return;
		}

		case FSM_BUZZER_STATE_IDLE: {
			// Waiting for processing message
			if(buzzer_params->_enable && GetMessage(MSG_BUZZER_PROCESSING)) {
				// Get buzzer control params
				buzzer_params = GetMessageParam(MSG_BUZZER_PROCESSING);
				// Goto PGM processing
				FSM_state = FSM_BUZZER_STATE_PGM_PROCESSING;
				ResetTimer(TIMER_BUZZER);
			}
			return;
		}


		default: break;
	}

}
