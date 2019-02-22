/*
 * x1226.c
 *
 * Created: 23.01.2019 12:49:22
 *  Author: fly8r
 */
#include "include/x1226.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t				FSM_state;
static					x1226_ccr_data_t	x1226;
static					uint8_t				_read_enabled=1; //, _x1226_init;


/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_X1226_Init(void)
{
	// Goto wait startup state
	FSM_state = FSM_X1226_STATE_STARTUP_TIMEOUT;
	// Flush FSM timer
	ResetTimer(TIMER_X1226);
}

void FSM_X1226_Process(void)
{
	switch(FSM_state) {
		/* Waiting timeout before initialize */
		case FSM_X1226_STATE_STARTUP_TIMEOUT: {
			if(GetTimer(TIMER_X1226) >= X1226_STARTUP_DELAY) {
				// Goto reading rtc status
				FSM_state = FSM_X1226_STATE_READ_STATUS;
			}
			return;
		}

		/* Getting STATUS info from X1226 */
		case FSM_X1226_STATE_READ_STATUS: {
			if(!i2c._busy) {
				if(I2C_RequestFrom(X1226_CCR_ADDRESS, 1, 2, X1226_REG_STATUS)) {
					// Goto status processing
					FSM_state = FSM_X1226_STATE_STATUS_PROCESSING;
				}
			}
			return;
		}

		/* Processing by status reg value */
		case FSM_X1226_STATE_STATUS_PROCESSING: {
			if(!i2c._busy) {
				if(!i2c._error) {
					// Set presence flag
					rtc._presence=1;
					// Action by status register data bit
					if(I2C_Read() & _RTCF) { // <- Total power fail occured
						// Set initialization type to FULL init
						x1226.init_type = X1226_INIT_TYPE_FULL;
					} else { // <- Power was not have total power fail
						// Set initialization type to PARTIAL init
						x1226.init_type = X1226_INIT_TYPE_PARTIAL;
					}
				} else {
					// Flush presence flag
					rtc._presence=0;
				}
				// Goto default state
				FSM_state = FSM_X1226_STATE_IDLE;
			}
			return;
		}

		/* 1 of 2 steps write protection off */
		case FSM_X1226_STATE_ENABLE_CCR_WRITE_ACCESS_STEP1: {
			if(!i2c._busy) {
				// Send 0x2(_WEL bit set) to the X1226
				uint8_t t = _WEL;
				I2C_TransmitTo(&t, X1226_CCR_ADDRESS, 1, 2, X1226_REG_STATUS);
				// Goto to second step write protection
				FSM_state = FSM_X1226_STATE_ENABLE_CCR_WRITE_ACCESS_STEP2;
			}
			return;
		}

		/* 2 of 2 steps write protection off */
		case FSM_X1226_STATE_ENABLE_CCR_WRITE_ACCESS_STEP2: {
			if(!i2c._busy) {
				// Send 0x6(_WEL & _RWEL bit set) to the X1226
				uint8_t t = (_WEL|_RWEL);
				I2C_TransmitTo(&t, X1226_CCR_ADDRESS, 1, 2, X1226_REG_STATUS);

				// Store copy init type
				enum X1226_INIT_TYPES_ENUM	curr_init_type = x1226.init_type;
				x1226.init_type = X1226_INIT_TYPE_NO_NEED;

				switch(curr_init_type) {
					/* Need full rtc initialization */
					case X1226_INIT_TYPE_FULL: {
						// Goto to setup clock date/time default values
						FSM_state = FSM_X1226_STATE_INIT_CLOCK;
						return;
					}
					/* Need partial rtc initialization */
					case X1226_INIT_TYPE_PARTIAL: {
						// Goto to setup EEPROM params to enable pulse interrupt mode for 1Hz IRQ output
						FSM_state = FSM_X1226_STATE_SET_EEPROM_PARAMS;
						return;
					}
					/* Store date data into RTC */
					case X1226_INIT_TYPE_SET_DATE: {
						// Goto store date data into RTC
						FSM_state = FSM_X1226_STATE_SET_DATE;
						return;
					}
					/* Store time data into RTC */
					case X1226_INIT_TYPE_SET_TIME: {
						// Goto store time data into RTC
						FSM_state = FSM_X1226_STATE_SET_TIME;
						return;
					}
					/* Need enable write mode */
					default: {
						// Goto to default state
						FSM_state = FSM_X1226_STATE_IDLE;
						break;
					}
				}
			}
			return;
		}

		/* Setup clock date/time default values */
		case FSM_X1226_STATE_INIT_CLOCK: {
			if(i2c._busy) { return; }
			// Store default values
			x1226.ccr.sec = dec2bcd(0);
			x1226.ccr.min = dec2bcd(DEFAULT_TIME_MINUTE);
			x1226.ccr.hour = (dec2bcd(DEFAULT_TIME_HOUR) | _MIL);	// Set 24hr time format
			x1226.ccr.day = dec2bcd(DEFAULT_DATE_DAY);
			x1226.ccr.month = dec2bcd(DEFAULT_DATE_MONTH);
			x1226.ccr.year = dec2bcd(DEFAULT_DATE_YEAR);
			x1226.ccr.dow = DEFAULT_DATE_DOW;
			// Send 6 byte date/time configuration values
			I2C_TransmitTo((void *)&x1226.ccr, X1226_CCR_ADDRESS, 7, 2, X1226_REG_RTC_SC);
			// Goto EEPROM params setup
			FSM_state = FSM_X1226_STATE_SET_EEPROM_PARAMS;
			return;
		}

		/* Custom params to use RTC x1226 */
		case FSM_X1226_STATE_SET_EEPROM_PARAMS: {
			if(!i2c._busy) {
				// Set 1Hz output to IRQ pin
				uint8_t t = (_FO1|_FO0);
				I2C_TransmitTo(&t, X1226_CCR_ADDRESS, 1, 2, X1226_REG_CTRL_INT);
				// Goto EEPROM write timeout
				FSM_state = FSM_X1226_STATE_SET_EEPROM_PARAMS_TIMEOUT;
				// Flush FSM timer for timeout
				ResetTimer(TIMER_X1226);
			}
			return;
		}

		/* EEPROM operation timeout state */
		case FSM_X1226_STATE_SET_EEPROM_PARAMS_TIMEOUT: {
			if(GetTimer(TIMER_X1226) >= X1226_EEPROM_WRITE_TIMEOUT) {
				//
				x1226.init_type = X1226_INIT_TYPE_NO_NEED;
				// Goto default work state
				FSM_state = FSM_X1226_STATE_IDLE;
				//
				ResetTimer(TIMER_X1226);
			}
			return;
		}

		/* Receive CCR data from X1226 */
		case FSM_X1226_STATE_READ_CCR_DATA: {
			// Disable read processing when message was received
			if(GetMessage(MSG_RTC_STOP_READ)) {
				// Disable read data
				_read_enabled=0;
				// Set next FSM state
				FSM_state = FSM_X1226_STATE_IDLE;
				return;
			}

			if(!i2c._busy) {
				if(I2C_RequestFrom(X1226_CCR_ADDRESS, 7, 2, X1226_REG_RTC_SC)) {
					// Goto processing received data
					FSM_state = FSM_X1226_STATE_CCR_DATA_PROCESSING;
				}
			}
			return;
		}

		/* Processing received data */
		case FSM_X1226_STATE_CCR_DATA_PROCESSING: {
			// Disable read processing when message was received
			if(GetMessage(MSG_RTC_STOP_READ)) {
				// Disable read data
				_read_enabled=0;
				// Set next FSM state
				FSM_state = FSM_X1226_STATE_IDLE;
				return;
			}

			if(!i2c._busy) {
				// Receive data from I2C buffer
				uint8_t *p_data = (void *)&x1226.ccr.sec;
				for(uint8_t i=7; i; i--) {
					*(p_data++) = I2C_Read();
				}
				// Processing received data
				rtc.sec = bcd2dec(x1226.ccr.sec);
				rtc.min = bcd2dec(x1226.ccr.min);
				rtc.hour = bcd2dec(x1226.ccr.hour & ~_MIL);
				rtc.dow = x1226.ccr.dow;
				rtc.day = bcd2dec(x1226.ccr.day);
				rtc.month = bcd2dec(x1226.ccr.month);
				rtc.year = bcd2dec(x1226.ccr.year);
				// Goto default work state
				FSM_state = FSM_X1226_STATE_IDLE;
			}
			return;
		}

		/* Store date data into RTC */
		case FSM_X1226_STATE_SET_DATE: {
			// Waiting for I2C bus free
			if(!i2c._busy) {
				// Prepare DATE data
				x1226.ccr.dow = rtc.dow;
				x1226.ccr.day = dec2bcd(rtc.day);
				x1226.ccr.month = dec2bcd(rtc.month);
				x1226.ccr.year = dec2bcd(rtc.year);
				// Set data to RTC
				I2C_TransmitTo(&x1226.ccr.day, X1226_CCR_ADDRESS, 4, 2, X1226_REG_RTC_DT);
				// Enable read data
				_read_enabled=1;
				// Flush FSM timer
				ResetTimer(TIMER_X1226);
				// Set next FSM state
				FSM_state = FSM_X1226_STATE_IDLE;
			}
			return;
		}

		case FSM_X1226_STATE_SET_TIME: {
			// Waiting for I2C bus free
			if(!i2c._busy) {
				// Prepare TIME data
				x1226.ccr.sec = dec2bcd(rtc.sec);
				x1226.ccr.min = dec2bcd(rtc.min);
				x1226.ccr.hour = (dec2bcd(rtc.hour) | _MIL);
				// Set data to RTC
				I2C_TransmitTo(&x1226.ccr.sec, X1226_CCR_ADDRESS, 3, 2, X1226_REG_RTC_SC);
				// Enable read data
				_read_enabled=1;
				// Flush FSM timer
				ResetTimer(TIMER_X1226);
				// Set next FSM state
				FSM_state = FSM_X1226_STATE_IDLE;
			}
			return;
		}

		case FSM_X1226_STATE_IDLE: {

			if(!rtc._presence) {
				return;
			}

			if(x1226.init_type != X1226_INIT_TYPE_NO_NEED) {
				// Goto write access set procedure
				FSM_state = FSM_X1226_STATE_ENABLE_CCR_WRITE_ACCESS_STEP1;
				return;
			}

			if(GetMessage(MSG_RTC_STOP_READ)) {
				// Disable read data
				_read_enabled=0;
			}

			if(GetMessage(MSG_RTC_RESUME_READ)) {
				// Enable read data
				_read_enabled=1;
			}

			if(GetMessage(MSG_RTC_SET_DATE)) {
				// Set next FSM state
				FSM_state = FSM_X1226_STATE_ENABLE_CCR_WRITE_ACCESS_STEP1;
				x1226.init_type = X1226_INIT_TYPE_SET_DATE;
				return;
			}

			if(GetMessage(MSG_RTC_SET_TIME)) {
				// Set next FSM state
				FSM_state = FSM_X1226_STATE_ENABLE_CCR_WRITE_ACCESS_STEP1;
				x1226.init_type = X1226_INIT_TYPE_SET_TIME;
				return;
			}


			if(_read_enabled && GetTimer(TIMER_X1226) >= X1226_DATA_READ_PERIOD) {
				FSM_state = FSM_X1226_STATE_READ_CCR_DATA;
				ResetTimer(TIMER_X1226);
			}

			return;
		}



		default: break;
	}
}
