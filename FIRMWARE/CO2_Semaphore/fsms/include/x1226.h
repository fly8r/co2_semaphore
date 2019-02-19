/*
 * x1226.h
 *
 * Created: 23.01.2019 12:49:08
 *  Author: fly8r
 */
#ifndef FSM_X1226_H_
#define FSM_X1226_H_

#include <avr/eeprom.h>

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"
#include "../../libs/include/tools.h"
#include "../../drivers/include/i2c.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_X1226_STATES_ENUM
{
	FSM_X1226_STATE_IDLE=0,
	FSM_X1226_STATE_STARTUP_TIMEOUT,
	FSM_X1226_STATE_READ_STATUS,
	FSM_X1226_STATE_STATUS_PROCESSING,
	FSM_X1226_STATE_ENABLE_CCR_WRITE_ACCESS_STEP1,
	FSM_X1226_STATE_ENABLE_CCR_WRITE_ACCESS_STEP2,
	FSM_X1226_STATE_INIT_CLOCK,
	FSM_X1226_STATE_SET_EEPROM_PARAMS,
	FSM_X1226_STATE_SET_EEPROM_PARAMS_TIMEOUT,
	FSM_X1226_STATE_READ_CCR_DATA,
	FSM_X1226_STATE_CCR_DATA_PROCESSING,

	FSM_X1226_STATE_READ_CONTROL_DATA,
	FSM_X1226_STATE_CONTROL_DATA_PROCESSING,


	FSM_X1226_STATE_SET_DATA,
	FSM_X1226_STATE_SET_TIME,
};

enum X1226_INIT_TYPES_ENUM
{
	X1226_INIT_TYPE_NO_NEED=0,		// No need initialization
	X1226_INIT_TYPE_PARTIAL,		// Need partial initialization (enable write, enable Pulse Interrupt Mode)
	X1226_INIT_TYPE_FULL			// Need full initialization (enable write, setup default clock data, enable Pulse Interrupt Mode)
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
#define			X1226_STARTUP_DELAY					20		// ms
#define			X1226_DATA_READ_PERIOD				500		// ms
#define			X1226_EEPROM_WRITE_TIMEOUT			12		// ms

#define			_WEL								(1<<1)
#define			_RWEL								(1<<2)
#define			_MIL								(1<<7)
#define			_RTCF								(1<<0)
#define			_FO0								(1<<3)
#define			_FO1								(1<<4)
#define			_IM									(1<<7)

// Memory mapping
#define			X1226_REG_STATUS					0x3F
#define			X1226_REG_RTC_Y2K					0x37
#define			X1226_REG_RTC_DW					0x36
#define			X1226_REG_RTC_YR					0x35
#define			X1226_REG_RTC_MO					0x34
#define			X1226_REG_RTC_DT					0x33
#define			X1226_REG_RTC_HR					0x32
#define			X1226_REG_RTC_MN					0x31
#define			X1226_REG_RTC_SC					0x30
#define			X1226_REG_CTRL_DTR					0x13
#define			X1226_REG_CTRL_ATR					0x12
#define			X1226_REG_CTRL_INT					0x11
#define			X1226_REG_CTRL_BL					0x10



/************************************************************************/
/* External VARS                                                        */
/************************************************************************/
typedef struct {
	enum	X1226_INIT_TYPES_ENUM	init_type;
	struct {
		uint8_t		sec,
					min,
					hour,
					day,
					month,
					year,
					dow,
					hyear,
					status;
	} ccr;
	struct {
		uint8_t		bl,
					int_d,
					atr,
					dtr;
	} ctrl;
} x1226_ccr_data_t;


/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_X1226_Init(void);
// Processing FSM function
void FSM_X1226_Process(void);



#endif /* FSM_X1226_H_ */