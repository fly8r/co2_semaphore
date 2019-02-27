/*
 * pcf8574.h
 *
 * Created: 23.01.2019 11:52:32
 *  Author: fly8r
 */
#ifndef FSM_PCF8574_H_
#define FSM_PCF8574_H_

#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include "../../global_vars.h"
#include "../../config.h"
#include "../../HAL.h"
#include "../../libs/include/messages.h"
#include "../../libs/include/swtimers.h"
#if (PCF8574_USE_CIRILLIC_TABLE)
	#include "../../libs/include/hd44780_cirillic.h"
#endif
#include "../../drivers/include/i2c.h"

/************************************************************************/
/* FSM states                                                           */
/************************************************************************/
enum FSM_PCF8574_STATES_ENUM
{
	FSM_PCF8574_STATE_IDLE=0,
	FSM_PCF8574_STATE_INIT_LCD,
	FSM_PCF8574_STATE_QUEUE_PROCESSING,
	FSM_PCF8574_STATE_SEND_BYTE,
};

/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
#define PCF8574_INIT_TIMEOUT					80		// HD44780 waiting internal process timeout in mS

//------------------------------ Display macros
#define PCF8574_COMMAND							0x0
#define PCF8574_DATA							0x80
#define PCF8574_BYTE_FULL                       0x0
#define PCF8574_BYTE_HALF                       0x40

//------------------------------ Display configuration flags
//>
#define	PCF8574_CMD_CLEAR_DISPLAY				0x01
#define PCF8574_CMD_FLUSH_DDRAM_ADDR			0x02
//>
#define PCF8574_CMD_DISPLAY_SHIFT_CURSOR		0x04
#define PCF8574_OPT_ADDRESS_DECREMENT			(PCF8574_CMD_DISPLAY_SHIFT_CURSOR & ~(1<<1))		// 0 - decrement address
#define PCF8574_OPT_ADDRESS_INCREMENT			(PCF8574_CMD_DISPLAY_SHIFT_CURSOR | (1<<1))			// 1 - increment address
#define PCF8574_OPT_LINE_SHIFT_DISABLE			(PCF8574_CMD_DISPLAY_SHIFT_CURSOR & ~(1<<0))		// 0 - no display shift
#define PCF8574_OPT_LINE_SHIFT_ENABLE			(PCF8574_CMD_DISPLAY_SHIFT_CURSOR |	(1<<0))			// 1 - shift display to the COUNTER_DIRECTION
//>
#define PCF8574_CMD_DISPLAY_MODE				0x08
#define PCF8574_OPT_DISPLAY_DISABLE				(PCF8574_CMD_DISPLAY_MODE & ~(1<<2))				// 0 - hidden all data
#define PCF8574_OPT_DISPLAY_ENABLE				(PCF8574_CMD_DISPLAY_MODE | (1<<2))					// 1 - show data
#define PCF8574_OPT_CURSOR_INVISIBLE			(PCF8574_CMD_DISPLAY_MODE & ~(1<<1))				// 0 - invisible
#define PCF8574_OPT_CURSOR_VISIBLE				(PCF8574_CMD_DISPLAY_MODE | (1<<1))					// 1 - visible
#define PCF8574_OPT_CURSOR_IS_UNDERLINE			(PCF8574_CMD_DISPLAY_MODE & ~(1<<0))				// 0 - cursor is underline
#define PCF8574_OPT_CURSOR_IS_SQUARE			(PCF8574_CMD_DISPLAY_MODE | (1<<0))					// 1 - cursor is black square
//>
#define PCF8574_CMD_SHIFT_MODE					0x10
#define PCF8574_OPT_SHIFT_CURSOR				(PCF8574_CMD_SHIFT_MODE & ~(1<<3))					// 0 - shift cursor
#define PCF8574_OPT_SHIFT_DISPLAY				(PCF8574_CMD_SHIFT_MODE | (1<<3))					// 1 - shift display
#define PCF8574_OPT_SHIFT_LEFT					(PCF8574_CMD_SHIFT_MODE & ~(1<<2))					// 0 - to the left
#define PCF8574_OPT_SHIFT_RIGHT					(PCF8574_CMD_SHIFT_MODE | (1<<2))					// 1 - to the right
//>
#define PCF8574_CMD_DATA_MODE					0x20
#define PCF8574_OPT_4BIT_MODE					(PCF8574_CMD_DATA_MODE & ~(1<<4))					// 0 - 4-bit mode
#define PCF8574_OPT_8BIT_MODE					(PCF8574_CMD_DATA_MODE | (1<<4))					// 1 - 8-bit mode
#define PCF8574_OPT_ONE_LINE_MODE				(PCF8574_CMD_DATA_MODE & ~(1<<3))					// 0 - one string
#define PCF8574_OPT_TWO_LINE_MODE				(PCF8574_CMD_DATA_MODE | (1<<3))					// 1 - two string
#define PCF8574_OPT_CHAR_SIZE_5X8				(PCF8574_CMD_DATA_MODE & ~(1<<2))					// 0 - 5x8 points
#define PCF8574_OPT_CHAR_SIZE_5X10				(PCF8574_CMD_DATA_MODE | (1<<2))					// 1 - 5x10 points
//>
#define PCF8574_CMD_CGRAM_ADDR					0x40
#define PCF8574_CMD_DDRAM_ADDR					0x80

/************************************************************************/
/* External VARS                                                        */
/************************************************************************/
typedef struct
{
	uint8_t		_half,
	type,
	timeout,
	byte;
} pcf8574_lcd_data_t;

typedef struct
{
	uint8_t			_busy,
	last_row;
	struct
	{
		uint8_t		p_write,
		p_send;

		#if (PCF8574_QUEUE_SIZE >= 255)
		uint16_t	length;
		#else
		uint8_t		length;
		#endif

		struct
		{
			char	byte;
			uint8_t type;
		} data[PCF8574_QUEUE_SIZE+1];
	} queue;
} pcf8574_data_t;
extern		pcf8574_data_t		pcf8574;


/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
// Main FSM initialize function
void FSM_PCF8574_Init(void);
// Processing FSM function
void FSM_PCF8574_Process(void);
// Add byte to queue
void FSM_PCF8574_AddByteToQueue(uint8_t byte, uint8_t type, uint8_t f_half, const uint8_t pause);
// Create lcd char
void FSM_PCF8574_CreateCharacter(char code, char *pattern);
// Create lcd char from flash
void FSM_PCF8574_CreateCharacterFromFlash(char code, const char *pattern);
// Set cursor to position
void FSM_PCF8574_GoToXY(uint8_t row, uint8_t col);
// Add string to LCD
void FSM_PCF8574_AddString(char * str, uint8_t start_row, uint8_t start_col);
// Add string from flash to LCD
void FSM_PCF8574_AddStringFromFlash(const char * str, uint8_t start_row, uint8_t start_col);
// Add char to LCD
void FSM_PCF8574_AddRAWChar(char chr);
// Add char from flash to LCD
void FSM_PCF8574_AddRAWCharFromFlash(const char * chr);
// Clear LCD display
void FSM_PCF8574_Clear(void);
// Flush FSM timer
void FSM_PCF8574_FlushTimer(void);


#endif /* FSM_PCF8574_H_ */
