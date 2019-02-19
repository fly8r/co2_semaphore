/*
 * mhz19b.c
 *
 * Created: 02.02.2019 13:06:20
 *  Author: fly8r
 */
#include "include/mhz19b.h"


/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;
const					char			cmd_set_range[9] PROGMEM  = { 0xFF, 0x1, 0x99, 0, 0, 0, 0x13, 0x88, 0xCB },
										cmd_set_abc[9] PROGMEM = { 0xFF, 0x1, 0x79, 0, 0, 0, 0, 0, 0x86 },  // Byte 3: 0xA0(crc: 0xE6) - ABC on, 0(crc: 0x86) - ABC off
										cmd_get_data[9] PROGMEM = { 0xFF, 0x1, 0x86, 0, 0, 0, 0, 0, 0x79 };
static					uint8_t			_first_run;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_MHZ19B_ReadDataComplete(void);
void FSM_MHZ19B_ReadDataError(void);
uint8_t FSM_MHZ19B_crc(uint8_t *buffer, uint8_t length);

/* Initialize FSM */
void FSM_MHZ19B_Init(void)
{
	// Flush flags
	mhz19b._presence=0;
	mhz19b._error=0;
	_first_run=1;
	// Goto initialization FSM
	FSM_state = FSM_MHZ19B_STATE_STARTUP_INIT;
}

/* Processing FSM */
void FSM_MHZ19B_Process(void)
{
	switch(FSM_state) {
		/* Send initialization command to the MH-Z19B */
		case FSM_MHZ19B_STATE_STARTUP_INIT: {
			UART_RX_INTERRUPT_DISABLE();
			// Sending set range CMD to the sensor
			UART_SendDataFromFlash(cmd_set_range, 9);
			// Sending ABC calibration settings to the sensor
			UART_SendDataFromFlash(cmd_set_abc, 9);
			// Goto waiting timeout before other actions
			FSM_state = FSM_MHZ19B_STATE_IDLE;
			ResetTimer(TIMER_MHZ19B);
			return;
		}
		/* Main state */
		case FSM_MHZ19B_STATE_IDLE: {
			/* Prepare read data from MH-Z19B by first run or timeout 20s */
			if(GetTimer(TIMER_MHZ19B) > MHZ19B_REFRESH_PERIOD) {
				// Get data command for MH-Z19B
				UART_SendDataFromFlash(cmd_get_data, 9);
				// Setup data length and process handlers
				UART_DefferedReadData(9, &FSM_MHZ19B_ReadDataComplete, &FSM_MHZ19B_ReadDataError);
				// Goto waiting data from MH-Z19B
				FSM_state = FSM_MHZ19B_STATE_GET_DATA;
				ResetTimer(TIMER_MHZ19B);
			}
			return;
		}

		/* Waiting data and processing */
		case FSM_MHZ19B_STATE_GET_DATA: {
			// Data was received without errors
			if(GetMessage(MSG_MHZ19B_DATA_RECEIVED)) {
				// Flush error flag
				mhz19b._error=0;
				// Goto data processing state
				FSM_state = FSM_MHZ19B_STATE_PROCESSING_DATA;
				return;
			}
			// Data was not received correctly
			if(GetMessage(MSG_MHZ19B_DATA_RECEIVE_ERROR)) {
				// Set error flag
				mhz19b._error=1;
				// Set error handle - DATA CORRUPT
				mhz19b.error=MHZ19B_ERROR_DATA_CORRUPT;
				FSM_state = FSM_MHZ19B_STATE_IDLE;
			}
			// Data was not received to the timeout end
			if(GetTimer(TIMER_MHZ19B) > MHZ19B_REFRESH_TIMEOUT) {
				// Set error flag
				mhz19b._error=1;
				// Set error handle - NO DEVICE
				mhz19b.error=MHZ19B_ERROR_NO_RESPONSE;
				FSM_state = FSM_MHZ19B_STATE_IDLE;
			}
			return;
		}
		/* Processing received data */
		case FSM_MHZ19B_STATE_PROCESSING_DATA: {
			uint8_t buff[9];
			uint8_t curr_crc;
			// Copy received data to temporary buffer
			for(uint8_t i=0; i<8; i++) {
				buff[i] = UART_Read();
			}
			curr_crc = UART_Read();
			// Check CRC match
			if(curr_crc != FSM_MHZ19B_crc(buff, 8)) {
				// Set error flag
				mhz19b._error=1;
				// Set error handle - CRC ERROR
				mhz19b.error=MHZ19B_ERROR_CRC;
				FSM_state = FSM_MHZ19B_STATE_IDLE;
				return;
			}
			// Get sensor value
			mhz19b.value = ((buff[2]<<8) + buff[3]);
			FSM_state = FSM_MHZ19B_STATE_IDLE;
			return;
		}
		default: break;
	}
}

uint8_t FSM_MHZ19B_crc(uint8_t *buffer, uint8_t length)
{
	uint8_t calc_crc=0;
	for(uint8_t i=1; i<length; i++) {
		calc_crc += buffer[i];
	}
	calc_crc = ~calc_crc + 1;
	return calc_crc;
}

void FSM_MHZ19B_ReadDataComplete(void)
{
	SendMessageWOParam(MSG_MHZ19B_DATA_RECEIVED);
}

void FSM_MHZ19B_ReadDataError(void)
{
	SendMessageWOParam(MSG_MHZ19B_DATA_RECEIVE_ERROR);
}