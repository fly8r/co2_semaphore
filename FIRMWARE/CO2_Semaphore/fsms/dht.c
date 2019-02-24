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
uint8_t FSM_DHT_ExpectHighToLowLevel(void);
uint8_t FSM_DHT_ExpectLowToHighLevel(void);

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

			if(GetTimer(TIMER_DHT) >= 2) {

				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					// Set port to INPUT
					DHT_DDR &= ~DHT_PIN_MASK;
					
					if(!FSM_DHT_ExpectHighToLowLevel()) {
						// Set error handler
						dht._failed = 1;
						// Set error handler
						dht._error = 1;
						dht.error = DHT_ERROR_NO_DEVICE;
						// Set next FSM state
						FSM_state = FSM_DHT_STATE_IDLE;
						return;
					}
					
					if(!FSM_DHT_ExpectLowToHighLevel()) {
						// Set error handler
						dht._error = 1;
						dht.error = DHT_ERROR_READY_TIMEOUT;
						// Set next FSM state
						FSM_state = FSM_DHT_STATE_IDLE;
						return;
					}
					
					if(!FSM_DHT_ExpectHighToLowLevel()) {
						// Set error handler
						dht._error = 1;
						dht.error = DHT_ERROR_TIMEOUT;
						// Set next FSM state
						FSM_state = FSM_DHT_STATE_IDLE;
						return;
					}
					

					// Receiving 5 bytes from DHT
					//uint8_t		*p_buff = (void *)&dht.data;
					for(uint8_t i=0; i<5; i++) {
						// Receiving procedure bit by bit
						uint8_t	tmp=0;
						for(uint8_t j=0; j<8; j++) {
							
							if(!FSM_DHT_ExpectLowToHighLevel()) {
								// Set error handler
								dht._error = 1;
								dht.error = DHT_ERROR_TIMEOUT;
								// Set next FSM state
								FSM_state = FSM_DHT_STATE_IDLE;
								return;
							}
							
							uint8_t t = FSM_DHT_ExpectHighToLowLevel();
							if(!t) {
								// Set error handler
								dht._error = 1;
								dht.error = DHT_ERROR_TIMEOUT;
								// Set next FSM state
								FSM_state = FSM_DHT_STATE_IDLE;
								return;
							}
							
							// Shift data bit in receive byte
							tmp <<= 1;
							// If slot has time over 50uS - is 1, else - 0
							if(t > 60) {
								tmp |= 0x1;
							}
						}
						// Store received byte to the buffer
						rx_buff[i] = tmp;
					}
				}
				// Check CRC
				if(rx_buff[4] == ((rx_buff[0] + rx_buff[1] + rx_buff[2] + rx_buff[3]) & 0xFF)) {
					FSM_state = FSM_DHT_STATE_PROCESSING;	
				} else {
					// Set error handler
					dht._error = 1;
					dht.error = DHT_ERROR_CRC;
					FSM_state = FSM_DHT_STATE_IDLE;
				}
			}
			return;
		}

		case FSM_DHT_STATE_PROCESSING: {

			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				
				dht.data.h = ((rx_buff[0] << 8) | rx_buff[1]); 
				dht.data.t = ((rx_buff[2] << 8) | rx_buff[3]);
				dht.data.crc = rx_buff[4]; 
				
				// Calculating DHT data values
				dht.temperature.sign = 0;
				if(dht.data.t & 0x8000) {
					dht.data.t &= 0x7FFF;
					dht.temperature.sign = 1;
				}
				dht.temperature.value = dht.data.t / 10;
				dht.temperature.mantissa = dht.data.t % 10;
				dht.humidity.value = dht.data.h / 10;
				dht.humidity.mantissa = dht.data.h % 10;
			}
			// Set next FSM state
			FSM_state = FSM_DHT_STATE_IDLE;
			return;
		}

		default: break;
	}

}

uint8_t FSM_DHT_ExpectHighToLowLevel(void)
{
	// Flush timer counter reg
	MEASURING_TMR_CNT = 0;
	// Waiting while level changed from HIGH to LOW
	while(DHT_PIN_STATE) {
		// Timeout control
		if(MEASURING_TMR_CNT > DHT_TIMESLOT_TIMEOUT) {
			return 0;
		}
	}
	return MEASURING_TMR_CNT;
}

uint8_t FSM_DHT_ExpectLowToHighLevel(void)
{
	// Flush timer counter reg
	MEASURING_TMR_CNT = 0;
	// Waiting while level changed from HIGH to LOW
	while(!DHT_PIN_STATE) {
		// Timeout control
		if(MEASURING_TMR_CNT > DHT_TIMESLOT_TIMEOUT) {
			return 0;
		}
	}
	return MEASURING_TMR_CNT;
}