/*
 * pcf8574.c
 *
 * Created: 23.01.2019 11:55:43
 *  Author: fly8r
 */
#include "include/pcf8574.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
volatile	static		uint8_t			FSM_state;

static		pcf8574_lcd_data_t			pcf8574_byte;
			pcf8574_data_t				pcf8574;


/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void FSM_PCF8574_AddByteToQueue(uint8_t byte, uint8_t type, uint8_t f_half, const uint8_t pause);
void FSM_PCF8574_CreateCharacter(char code, char *pattern);
void FSM_PCF8574_CreateCharacterFromFlash(char code, const char *pattern);
void FSM_PCF8574_GoToXY(uint8_t row, uint8_t col);
void FSM_PCF8574_AddString(char * str);
void FSM_PCF8574_AddStringFromFlash(const char * str);
void FSM_PCF8574_AddRAWChar(char chr);
void FSM_PCF8574_AddRAWCharFromFlash(const char * chr);
void FSM_PCF8574_Clear(void);
void FSM_PCF8574_FlushTimer(void);


/* Initialize FSM */
void FSM_PCF8574_Init(void)
{
	// Set next FSM state
	FSM_state = FSM_PCF8574_STATE_INIT_LCD;
	// Flush FSM timer
	ResetTimer(TIMER_PCF8574);
}

/* Processing FSM */
void FSM_PCF8574_Process(void)
{
	switch(FSM_state) {
		case FSM_PCF8574_STATE_IDLE: {
			// Something need to transmit
			if(pcf8574.queue.length) {
				// Flush timeout for the next byte
				pcf8574_byte.timeout = 0;
				// Set next FSM state
				FSM_state = FSM_PCF8574_STATE_QUEUE_PROCESSING;
			}
			return;
		}

		case FSM_PCF8574_STATE_QUEUE_PROCESSING: {
			// Check I2C BUSY and lcd BUSY flags
			if(i2c._busy) {
				ResetTimer(TIMER_PCF8574);
				return;
			}
			// Waiting for timeout complete
			if(!pcf8574_byte.timeout || GetTimer(TIMER_PCF8574) > pcf8574_byte.timeout) {
				if(pcf8574.queue.length) {
					// Decrement output queue
					pcf8574.queue.length--;
					// Extracting data type info
					pcf8574_byte.type = (pcf8574.queue.data[pcf8574.queue.p_send].type & PCF8574_DATA);
					// Extracting byte consistence info
					pcf8574_byte._half = (pcf8574.queue.data[pcf8574.queue.p_send].type & PCF8574_BYTE_HALF);
					// Set timeout for current operation
					pcf8574_byte.timeout = (pcf8574.queue.data[pcf8574.queue.p_send].type & 0x3F);
					// Copy data from query to output buffer
					pcf8574_byte.byte = pcf8574.queue.data[pcf8574.queue.p_send].byte;
					// Control overflow by cycling buffer
					if(pcf8574.queue.p_send++ >= PCF8574_QUEUE_SIZE) pcf8574.queue.p_send=0;
					// Set lcd BUSY flag
					pcf8574._busy = 1;
					// Send current byte from queue
					FSM_state = FSM_PCF8574_STATE_SEND_BYTE;
					return;
				}
				// Flush lcd BUSY flag
				pcf8574._busy = 0;
				// Set next FSM state
				FSM_state = FSM_PCF8574_STATE_IDLE;
			}
			return;
		}

		case FSM_PCF8574_STATE_SEND_BYTE: {
			// Waiting for I2C bus free
			if(i2c._busy) return;
			// Processing data byte for transmit
			uint8_t i=0;
			uint8_t buff[4];
			do {
				//uint8_t buff = 0;
				// If need to send only half of byte
				if(pcf8574_byte._half && i == 2) break;
				// Prepare data for set in buffer
				switch(i) {
					case 0: case 1: buff[i] = (pcf8574_byte.byte & 0xF0); break;
					case 2: case 3: buff[i] = ((pcf8574_byte.byte << 4) & 0xF0); break;
				}
				// If data type is DATA, set RS level
				if(pcf8574_byte.type == PCF8574_DATA) {
					buff[i] |= PCF8574_PIN_RS_MASK;
				}
				// Imitation strobe on pin E in even byte
				if(!(i & 0x1)) {
					buff[i] |= PCF8574_PIN_E_MASK;
				}
				#if(PCF8574_BL_CTRL)
					buff[i] |= PCF8574_PIN_BL_MASK;
				#endif
				// Increment byte counter
				i++;
			} while(i < 4);
			// Transmit data
			I2C_TransmitTo(buff, PCF8574_ADDRESS, i, 0, 0);
			// Set next FSM state
			FSM_state = FSM_PCF8574_STATE_QUEUE_PROCESSING;
			return;
		}

		case FSM_PCF8574_STATE_INIT_LCD: {
			// Waiting for HD44780 initialization complete
			if(GetTimer(TIMER_PCF8574) < PCF8574_INIT_TIMEOUT) {
				return;
			}

			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				// Initialize queue buffer
				for(uint16_t i=0; i<PCF8574_QUEUE_SIZE; i++) {
					pcf8574.queue.data[i].byte=0;
					pcf8574.queue.data[i].type=0;
				}
			}

			// Step.1. Sequence of initialization commands: 1 of 4
			FSM_PCF8574_AddByteToQueue(PCF8574_OPT_8BIT_MODE, PCF8574_COMMAND, PCF8574_BYTE_HALF, 20);
			FSM_PCF8574_AddByteToQueue(PCF8574_OPT_8BIT_MODE, PCF8574_COMMAND, PCF8574_BYTE_HALF, 5);
			// Step.2. Sequence of initialization commands: 2 of 4
			FSM_PCF8574_AddByteToQueue(PCF8574_OPT_8BIT_MODE, PCF8574_COMMAND, PCF8574_BYTE_HALF, 1);
			// Step.3. Sequence of initialization commands: 3 of 4
			FSM_PCF8574_AddByteToQueue(PCF8574_OPT_4BIT_MODE, PCF8574_COMMAND, PCF8574_BYTE_HALF, 0);
			// Step.4. Sequence of initialization commands: 4 of 4
			FSM_PCF8574_AddByteToQueue(PCF8574_OPT_4BIT_MODE | PCF8574_OPT_TWO_LINE_MODE | PCF8574_OPT_CHAR_SIZE_5X8, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
			// Step.5. Command to disable display
			FSM_PCF8574_AddByteToQueue(PCF8574_OPT_DISPLAY_DISABLE, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
			// Step.6. Command for clear display
			FSM_PCF8574_AddByteToQueue(PCF8574_CMD_CLEAR_DISPLAY, PCF8574_COMMAND, PCF8574_BYTE_FULL, 2);
			// Step.7. Enable display and cursor tunning
			FSM_PCF8574_AddByteToQueue(PCF8574_OPT_DISPLAY_ENABLE | PCF8574_OPT_CURSOR_INVISIBLE, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
			// Step.8. Set address counter direction and display shifting
			FSM_PCF8574_AddByteToQueue(PCF8574_OPT_ADDRESS_INCREMENT | PCF8574_OPT_LINE_SHIFT_DISABLE, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
			// Set next FSM state
			FSM_state = FSM_PCF8574_STATE_IDLE;
			// Notify another FSMs that LCD init instruction was send
			SendBCMessage(MSG_BC_PCF8574_INIT_CMD_SEND_COMPLETE);
			return;
		}


		default: break;
	}
}

/* Add byte to queue */
void FSM_PCF8574_AddByteToQueue(uint8_t byte, uint8_t type, uint8_t f_half, const uint8_t pause)
{
	uint8_t	timeout=0;

	if(pcf8574.queue.length >= PCF8574_QUEUE_SIZE) {
		//@TODO: Set error info
		return;
	}
	// Summering flags
	type |= f_half;
	// Timeout max limit is 64ms
	timeout = (pause & 0x3F);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		// Store byte into queue
		pcf8574.queue.data[pcf8574.queue.p_write].byte = byte;
		// Store additional params for data byte transmission
		pcf8574.queue.data[pcf8574.queue.p_write].type = (type | timeout);
		// Increment queue length
		pcf8574.queue.length++;
		// Control overflow by cycling buffer
		if(pcf8574.queue.p_write++ >= PCF8574_QUEUE_SIZE) pcf8574.queue.p_write=0;
	}
}

/* Create lcd char */
void FSM_PCF8574_CreateCharacter(char code, char *pattern)
{
	// Set CGRAM address pointer shift
	FSM_PCF8574_AddByteToQueue(PCF8574_CMD_CGRAM_ADDR | (code << 3), PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
	// Load pattern in CGRAM 8 bytes line by line
	for(uint8_t i=0; i<8; i++) {
		FSM_PCF8574_AddByteToQueue(*(pattern++), PCF8574_DATA, PCF8574_BYTE_FULL, 0);
	}
	// Return to DDRAM in position 0,0
	FSM_PCF8574_AddByteToQueue(PCF8574_CMD_DDRAM_ADDR, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
}

/* Create lcd char from flash */
void FSM_PCF8574_CreateCharacterFromFlash(char code, const char *pattern)
{
	// Set CGRAM address pointer shift
	FSM_PCF8574_AddByteToQueue(PCF8574_CMD_CGRAM_ADDR | (code << 3), PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
	// Load pattern in CGRAM 8 bytes line by line
	for(uint8_t i=0; i<8; i++) {
		FSM_PCF8574_AddByteToQueue(pgm_read_byte(pattern++), PCF8574_DATA, PCF8574_BYTE_FULL, 0);
	}
	// Return to DDRAM in position 0,0
	FSM_PCF8574_AddByteToQueue(PCF8574_CMD_DDRAM_ADDR, PCF8574_COMMAND, PCF8574_BYTE_FULL, 0);
}

/* Set cursor to position */
void FSM_PCF8574_GoToXY(uint8_t row, uint8_t col)
{
	uint8_t addr=0;
	if(row & 0x1) addr=0x40;				// Odd row is similarly has address 0x40
	if(row > 1) addr += PCF8574_COLS;		// For 4 rows display and row > 1 add address shift
	// Calculate address with Col shift
	addr += col;
	// Set address to display DRAM
	FSM_PCF8574_AddByteToQueue(PCF8574_CMD_DDRAM_ADDR | addr, PCF8574_COMMAND, PCF8574_BYTE_FULL, 2);
	// Store row value
	pcf8574.last_row = row;
}

/* Add string to LCD */
void FSM_PCF8574_AddString(char * str)
{
	char chr;
	while ((chr = *str)) {
		switch(chr) {
			// Goto new line
			case '\n': {
				if(pcf8574.last_row++ >= PCF8574_ROWS) pcf8574.last_row=0;
			}
			// Flush cursor position in row
			case '\r': {
				FSM_PCF8574_GoToXY(pcf8574.last_row, 0);
				break;
			}
			// Tabulation replace with 4 space char
			case '\t': {
				for(uint8_t i=4; i; i--) {
					FSM_PCF8574_AddByteToQueue(0x20, PCF8574_DATA, PCF8574_BYTE_FULL, 0);
				}
			}

			default: {
				#if (PCF8574_USE_CIRILLIC_TABLE)
					FSM_PCF8574_AddByteToQueue(HD44780_CirillicTable(chr), PCF8574_DATA, PCF8574_BYTE_FULL, 0);
				#else
					FSM_PCF8574_AddByteToQueue(chr, PCF8574_DATA, PCF8574_BYTE_FULL, 0);
				#endif
				break;
			}
		}
		str++;
	}
}

/* Add string from flash to LCD */
void FSM_PCF8574_AddStringFromFlash(const char * str)
{
	char chr;
	while ((chr = pgm_read_byte(str))) {
		switch(chr) {
			// Goto new line
			case '\n': {
				if(pcf8574.last_row++ >= PCF8574_ROWS) pcf8574.last_row=0;
			}
			// Flush cursor position in row
			case '\r': {
				FSM_PCF8574_GoToXY(pcf8574.last_row, 0);
				break;
			}
			// Tabulation replace with 4 space char
			case '\t': {
				for(uint8_t i=4; i; i--) {
					FSM_PCF8574_AddByteToQueue(0x20, PCF8574_DATA, PCF8574_BYTE_FULL, 0);
				}
			}

			default: {
				#if (PCF8574_USE_CIRILLIC_TABLE)
				FSM_PCF8574_AddByteToQueue(HD44780_CirillicTable(chr), PCF8574_DATA, PCF8574_BYTE_FULL, 0);
				#else
				FSM_PCF8574_AddByteToQueue(chr, PCF8574_DATA, PCF8574_BYTE_FULL, 0);
				#endif
				break;
			}
		}
		str++;
	}
}

/* Add char to LCD */
void FSM_PCF8574_AddRAWChar(char chr)
{
	FSM_PCF8574_AddByteToQueue(chr, PCF8574_DATA, PCF8574_BYTE_FULL, 0);
}

/* Add char from flash to LCD */
void FSM_PCF8574_AddRAWCharFromFlash(const char *chr)
{
	char c = pgm_read_byte(*chr);
	FSM_PCF8574_AddRAWChar(c);
}

/* Clear LCD display */
void FSM_PCF8574_Clear(void)
{
	FSM_PCF8574_AddByteToQueue(PCF8574_CMD_CLEAR_DISPLAY, PCF8574_COMMAND, PCF8574_BYTE_FULL, 2);
	pcf8574.last_row=0;
}
