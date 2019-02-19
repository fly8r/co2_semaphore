/*
 * dht.c
 *
 * Created: 19.11.2018 15:51:29
 *  Author: fly8r
 */
#include "include/dht.h"


/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;
static					uint8_t			rx_buff[5];

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_DHT_Init(void)
{
	// Initialize error var
	dht._failed = 0;
	dht.error = DHT_ERROR_NO_ERROR;
	// Set next FSM state
	FSM_state = FSM_DHT_STATE_IDLE;
	// Flush FSM timer
	ResetTimer(TIMER_DHT);
}


void FSM_DHT_Process(void)
{
	switch(FSM_state) {

		case FSM_DHT_STATE_IDLE: {
			// If no error before and time to refresh data
			if(!dht._failed && GetTimer(TIMER_DHT) > DHT_REFRESH_PERIOD) {
				// Set port pin in OUT mode
				DHT_DDR |= DHT_PIN_MASK;
				// Set port pin in low level
				DHT_PORT &= ~DHT_PIN_MASK;
				// Flush FSM timer
				ResetTimer(TIMER_DHT);
				// Set next FSM state
				FSM_state = FSM_DHT_STATE_GET_DATA;
			}
			return;
		}

		case FSM_DHT_STATE_GET_DATA: {

			if(GetTimer(TIMER_DHT) > 5) {

				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					// Set port pin in HIGH level
					DHT_PORT |= DHT_PIN_MASK;
					// Technology timeout ~20uS
					_delay_us(30);
					// Set port pin in IN mode
					DHT_DDR &= ~DHT_PIN_MASK;
					// Flush timer counter reg
					MEASURING_TMR_CNT = 0;
					// Waiting while level changed from LOW to HIGH
					while(!DHT_PIN_STATE) {
						// Timeout control
						if(MEASURING_TMR_CNT > DHT_TIMESLOT_TIMEOUT) {
							// Set error handler
							dht._failed = 1;
							dht.error = DHT_ERROR_NO_DEVICE;
							// Set next FSM state
							FSM_state = FSM_DHT_STATE_IDLE;
							return;
						}
					}

					// Flush timer counter reg
					MEASURING_TMR_CNT = 0;
					// Waiting while level changed from LOW to HIGH
					while(DHT_PIN_STATE) {
						// Timeout control
						if(MEASURING_TMR_CNT > DHT_TIMESLOT_TIMEOUT) {
							// Set error handler
							dht._failed = 1;
							dht.error = DHT_ERROR_TIMEOUT;
							// Set next FSM state
							FSM_state = FSM_DHT_STATE_IDLE;
							return;
						}
					}

					// Receiving 5 bytes from DHT
					//uint8_t		*p_buff = (void *)&dht.data;
					for(int8_t i=4; i>=0; i--) {
						// Receiving procedure bit by bit
						uint8_t	tmp=0;
						for(uint8_t b=8; b>0; b--) {
							// Flush timer counter reg
							MEASURING_TMR_CNT = 0;
							// Waiting while level changed from LOW to HIGH
							while(!DHT_PIN_STATE) {
								// Timeout control
								if(MEASURING_TMR_CNT > DHT_TIMESLOT_TIMEOUT) {
									// Set error handler
									dht._failed = 1;
									dht.error = DHT_ERROR_TIMEOUT;
									// Set next FSM state
									FSM_state = FSM_DHT_STATE_IDLE;
									return;
								}
							}
							// Flush timer counter reg
							MEASURING_TMR_CNT = 0;
							// Waiting while level changed from HIGH to LOW
							while(DHT_PIN_STATE) {
								// Timeout control
								if(MEASURING_TMR_CNT > DHT_TIMESLOT_TIMEOUT) {
									// Set error handler
									dht._failed = 1;
									dht.error = DHT_ERROR_TIMEOUT;
									// Set next FSM state
									FSM_state = FSM_DHT_STATE_IDLE;
									return;
								}
							}
							// Shift data bit in receive byte
							tmp <<= 1;
							// If slot has time over 50uS - is 1, else - 0
							if(MEASURING_TMR_CNT > 50) {
								tmp |= 0x1;
							}
						}
						// Store received byte to the buffer
						rx_buff[i] = tmp;
					}
				}
				FSM_state = FSM_DHT_STATE_PROCESSING;
			}
			return;
		}

		case FSM_DHT_STATE_PROCESSING: {


			uint8_t *p_data = (void *)&dht.data;
			uint8_t *p_buff = rx_buff;

			for(uint8_t i=5; i; i--) {
				*(p_data++) = *(p_buff++);
			}

			// Calculating DHT data values
			dht.temperature.sign = 0;
			if(dht.data.t & 0x8000) {
				dht.temperature.sign = 1;
			}
			dht.temperature.value = dht.data.t / 10;
			dht.temperature.mantissa = dht.data.t % 10;
			dht.humidity.value = dht.data.h / 10;
			dht.humidity.mantissa = dht.data.h % 10;
			// Set next FSM state
			FSM_state = FSM_DHT_STATE_IDLE;
			return;
		}

		default: break;
	}

}
