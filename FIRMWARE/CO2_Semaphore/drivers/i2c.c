/*
 * i2c.c
 *
 * Created: 23.01.2019 10:51:15
 *  Author: fly8r
 * Version: 1.1 (25.01.2019)
 */
#include "include/i2c.h"

/************************************************************************/
/* Prototype function                                                   */
/************************************************************************/
void DoNothing(void);

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
i2c_data_t		i2c;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
inline void I2C_TWIService(void)
{
	switch(TWSR & 0xF8) {

		// Bus error due to an illegal START or STOP condition
		case I2C_STATE_M_BUS_FAIL: {
			// Set I2C Error
			i2c.error = I2C_ERROR_BUS_FAIL;
			// Set TWI register flags
			TWCR	=	0<<TWSTA |					// Not START
						1<<TWSTO |					// Set STOP condition
						1<<TWINT |					// Flush TWI interrupt flag
						I2C_SLAVE_MODE<<TWEA |		// Enable ACK if SLAVE mode enabled
						1<<TWEN |					// Enable TWI operation
						1<<TWIE;					// Enable TWI interrupt
			// Flush I2C busy flag
			i2c._busy = 0;
			// Set I2C error flag
			i2c._error = 1;
			// Run error handling process
			I2C_PROC_ERROR_OUT();
			break;
		}

		// A START condition has been transmitted
		case I2C_STATE_M_START: {
			// Set R/W flag by mode
			if(i2c.transmit_type == I2C_TRANSMIT_TYPE_SARP) {
				// Set to 1(R) lower bit in address - READ MODE
				i2c.master.slave_addr |= 0x01;
			} else {
				// Set to 0(W) lower bit in address - WRITE MODE
				i2c.master.slave_addr &= 0xFE;
			}
			// Set SLAVE address
			TWDR = i2c.master.slave_addr;
			// Set TWI register flags
			TWCR	=	0<<TWSTA |					// Not START
						0<<TWSTO |					// Not STOP
						1<<TWINT |					// Flush TWI interrupt flag
						I2C_SLAVE_MODE<<TWEA |		// Enable ACK if SLAVE mode enabled
						1<<TWEN |					// Enable TWI operation
						1<<TWIE;					// Enable TWI interrupt
			break;
		}

		// A repeated START condition has been transmitted
		case I2C_STATE_M_RESTART: {
			// Set R/W flag by mode
			if(i2c.transmit_type == I2C_TRANSMIT_TYPE_SAWSARP) {
				// Set to 1(W) lower bit in address - READ MODE
				i2c.master.slave_addr |= 0x01;
			} else {
				// Set to 0(R) lower bit in address - WRITE MODE
				i2c.master.slave_addr &= 0xFE;
			}
			// Set SLAVE address
			TWDR = i2c.master.slave_addr;
			// Set TWI register flags
			TWCR	=	0<<TWSTA |					// Not START
						0<<TWSTO |					// Not STOP
						1<<TWINT |					// Flush TWI interrupt flag
						I2C_SLAVE_MODE<<TWEA |		// Enable ACK if SLAVE mode enabled
						1<<TWEN |					// Enable TWI operation
						1<<TWIE;					// Enable TWI interrupt
			break;
		}

		// SLA+W has been transmitted; ACK has been received
		case I2C_STATE_M_SLA_W_ACK: {
			// Prepare and send data by mode
			if(i2c.page.q_length) { // <- Need to send start page address word
				// Load page address from page address buffer to I2C output register
				TWDR = i2c.page.buff[i2c.page.p_buff];
				// Increment page address buffer and control overflow
				if(i2c.page.p_buff++ >= I2C_PAGE_WORD_LENGTH) i2c.page.p_buff=0;
				// Decrement page address queue
				i2c.page.q_length--;
			} else if(i2c.master.q_length) { // <- Directly send data
				// Prepare and send first data byte by mode
				// Load data from buffer to I2C output register
				TWDR = i2c.master.tx_buff[i2c.master.p_buff];
				// Increment buffer send index and control overflow
				if(i2c.master.p_buff++ >= I2C_M_LENGTH_TX_BUFFER) i2c.master.p_buff=0;
				// Decrement data byte counter queue
				i2c.master.q_length--;
				} else { // <- Another state is wrong - send STOP
				//> The byte is last
				// Set TWI register flags
				TWCR	=	0<<TWSTA |					// Not START
				1<<TWSTO |					// Set STOP condition
				1<<TWINT |					// Flush TWI interrupt flag
				I2C_SLAVE_MODE<<TWEA |		// Enable ACK if SLAVE mode enabled
				1<<TWEN |					// Enable TWI operation
				1<<TWIE;					// Enable TWI interrupt
				break;
			}
			// Set TWI register flags
			TWCR	=	0<<TWSTA |						// Not START
						0<<TWSTO |						// Not STOP
						1<<TWINT |						// Flush TWI interrupt flag
						I2C_SLAVE_MODE<<TWEA |			// Enable ACK if SLAVE mode enabled
						1<<TWEN |						// Enable TWI operation
						1<<TWIE;						// Enable TWI interrupt
			break;
		}

		// SLA+W has been transmitted; NOT ACK has been received
		case I2C_STATE_M_SLA_W_NACK: {
			// Set I2C Error
			i2c.error = I2C_ERROR_DEVICE_NOT_FOUND;
			// Set TWI register flags
			TWCR	=	0<<TWSTA |							// Not START
						1<<TWSTO |							// Set STOP condition
						1<<TWINT |							// Flush TWI interrupt flag
						I2C_SLAVE_MODE<<TWEA |				// Enable ACK if SLAVE mode enabled
						1<<TWEN |							// Enable TWI operation
						1<<TWIE;							// Enable TWI interrupt
			// Flush I2C busy flag
			i2c._busy = 0;
			// Set I2C error flag
			i2c._error = 1;
			// Run error handling process
			I2C_PROC_ERROR_OUT();
			break;
		}

		// Data byte has been transmitted to slave; ACK has been received
		case I2C_STATE_M_BYTE_ACK: {
			// Prepare and send data by mode
			if(i2c.transmit_type == I2C_TRANSMIT_TYPE_SAWP) {
				// If all data was transmitted
				if(i2c.page.q_length == 0 && i2c.master.q_length == 0) { // <- Send STOP condition
					//> The byte is last
					// Set TWI register flags
					TWCR	=	0<<TWSTA |					// Not START
								1<<TWSTO |					// Set STOP condition
								1<<TWINT |					// Flush TWI interrupt flag
								I2C_SLAVE_MODE<<TWEA |		// Enable ACK if SLAVE mode enabled
								1<<TWEN |					// Enable TWI operation
								1<<TWIE;					// Enable TWI interrupt
					// Flush I2C busy flag
					i2c._busy = 0;
					// Flush I2C error flag
					i2c._error = 0;
					// Run normal handling process
					I2C_PROC_MASTER_OUT();
					break;
				}

				// If page address queue is not empty
				if(i2c.page.q_length) { // <- Send page address byte
					// Load page address from page address buffer to I2C output register
					TWDR = i2c.page.buff[i2c.page.p_buff];
					// Increment page address buffer and control overflow
					if(i2c.page.p_buff++ >= I2C_PAGE_WORD_LENGTH) i2c.page.p_buff=0;
					// Decrement page address queue
					i2c.page.q_length--;
				} else if(i2c.master.q_length) { // <- Send next data byte
					//> The byte is not last
					// Load data from buffer to I2C output register
					TWDR = i2c.master.tx_buff[i2c.master.p_buff];
					// Increment buffer send index and control overflow
					if(i2c.master.p_buff++ >= I2C_M_LENGTH_TX_BUFFER) i2c.master.p_buff = 0;
					// Decrement data byte counter queue
					i2c.master.q_length--;
				}
				// Set TWI register flags
				TWCR	=	0<<TWSTA |					// Not START
							0<<TWSTO |					// Not STOP
							1<<TWINT |					// Flush TWI interrupt flag
							I2C_SLAVE_MODE<<TWEA |		// Enable ACK if SLAVE mode enabled
							1<<TWEN |					// Enable TWI operation
							1<<TWIE;					// Enable TWI interrupt
			}
			// Prepare and send data by mode
			if(i2c.transmit_type == I2C_TRANSMIT_TYPE_SAWSARP) {
				// Send page address by page address byte count
				if(i2c.page.q_length) {
					//> The page address byte was not last
					// Load page address from page address buffer to I2C output register
					TWDR = i2c.page.buff[i2c.page.p_buff];
					// Increment page address buffer
					if(i2c.page.p_buff++ >= I2C_PAGE_WORD_LENGTH) i2c.page.p_buff=0;
					// Decrement page address queue
					i2c.page.q_length--;
					// Set TWI register flags
					TWCR	=	0<<TWSTA |					// Not START
								0<<TWSTO |					// Not STOP
								1<<TWINT |					// Flush TWI interrupt flag
								I2C_SLAVE_MODE<<TWEA |		// Enable ACK if SLAVE mode enabled
								1<<TWEN |					// Enable TWI operation
								1<<TWIE;					// Enable TWI interrupt
				} else {
					//> Last byte of page address was send
					// Set TWI register flags
					TWCR	=	1<<TWSTA |					// Set RESTART condition
								0<<TWSTO |					// Not STOP
								1<<TWINT |					// Flush TWI interrupt flag
								I2C_SLAVE_MODE<<TWEA |		// Enable ACK if SLAVE mode enabled
								1<<TWEN |					// Enable TWI operation
								1<<TWIE;					// Enable TWI interrupt
				}
			}
			break;
		}

		// Data byte has been transmitted; NOT ACK has been received
		case I2C_STATE_M_BYTE_NACK: {
			// Set I2C Error
			i2c.error = I2C_ERROR_RECEIVED_NACK;
			// Set TWI register flags
			TWCR	=	0<<TWSTA |							// Not START
						1<<TWSTO |							// Set STOP condition
						1<<TWINT |							// Flush TWI interrupt flag
						I2C_SLAVE_MODE<<TWEA |				// Enable ACK if SLAVE mode enabled
						1<<TWEN |							// Enable TWI operation
						1<<TWIE;							// Enable TWI interrupt
			// Flush I2C busy flag
			i2c._busy = 0;
			// Set I2C error flag
			i2c._error = 1;
			// Run normal handling process
			I2C_PROC_MASTER_OUT();
			break;
		}

		// Arbitration lost in SLA+W or data bytes
		case I2C_STATE_M_COLLISION: {
			// Set I2C Error
			i2c.error = I2C_ERROR_LOW_PRIORITY;
			// Flush buffer pointers
			i2c.master.p_buff = 0;
			i2c.page.p_buff = 0;
			// Set TWI register flags
			TWCR	=	1<<TWSTA |							// Set START condition
						0<<TWSTO |							// Not STOP
						1<<TWINT |							// Flush TWI interrupt flag
						I2C_SLAVE_MODE<<TWEA |				// Enable ACK if SLAVE mode enabled
						1<<TWEN |							// Enable TWI operation
						1<<TWIE;							// Enable TWI interrupt
			// Flush I2C busy flag
			i2c._busy = 0;
			// Set I2C error flag
			i2c._error = 1;
			break;
		}

		// SLA+R has been transmitted; ACK has been received
		case I2C_STATE_M_SLA_R_ACK: {
			// If next byte will be last for receive
			if(i2c.master.q_length == 1) {
				//> Send next byte with NACK
				// Set TWI register flags
				TWCR	=	0<<TWSTA |						// Not START
							0<<TWSTO |						// Not STOP
							1<<TWINT |						// Flush TWI interrupt flag
							0<<TWEA |						// Set NACK condition
							1<<TWEN |						// Enable TWI operation
							1<<TWIE;						// Enable TWI interrupt
			} else {
				//> Send next byte with ACK
				// Set TWI register flags
				TWCR	=	0<<TWSTA |						// Not START
							0<<TWSTO |						// Not STOP
							1<<TWINT |						// Flush TWI interrupt flag
							1<<TWEA |						// Set ACK condition
							1<<TWEN |						// Enable TWI operation
							1<<TWIE;						// Enable TWI interrupt
			}
			break;
		}

		// SLA+R has been transmitted; NOT ACK has been received
		case I2C_STATE_M_SLA_R_NACK: {
			// Set I2C Error
			i2c.error = I2C_ERROR_DEVICE_NOT_FOUND;
			// Set TWI register flags
			TWCR	=	0<<TWSTA |							// Not START
						1<<TWSTO |							// Set STOP condition
						1<<TWINT |							// Flush TWI interrupt flag
						I2C_SLAVE_MODE<<TWEA |				// Enable ACK if SLAVE mode enabled
						1<<TWEN |							// Enable TWI operation
						1<<TWIE;							// Enable TWI interrupt
			// Flush I2C busy flag
			i2c._busy = 0;
			// Set I2C error flag
			i2c._error = 1;
			// Run error handling process
			I2C_PROC_ERROR_OUT();
			break;
		}

		// Data byte has been received by master; ACK has been returned
		case I2C_STATE_M_RECEIVE_BYTE: {
			// Store received byte into buffer
			i2c.master.rx_buff[i2c.master.p_buff] = TWDR;
			// Increment buffer index
			if(i2c.master.p_buff++ >= I2C_M_LENGTH_RX_BUFFER) i2c.master.p_buff = 0;
			// Decrement receive queue counter
			i2c.master.q_length--;
			// Receive next byte with ACK/NACK state
			if(i2c.master.q_length == 1) {
				//> Next byte will be last for receive
				// Set TWI register flags
				TWCR	=	0<<TWSTA |						// Not START
							0<<TWSTO |						// Not STOP
							1<<TWINT |						// Flush TWI interrupt flag
							0<<TWEA |						// Set NACK condition
							1<<TWEN |						// Enable TWI operation
							1<<TWIE;						// Enable TWI interrupt
			} else {
				//> Next byte will not be last for receive
				// Set TWI register flags
				TWCR	=	0<<TWSTA |						// Not START
							0<<TWSTO |						// Not STOP
							1<<TWINT |						// Flush TWI interrupt flag
							1<<TWEA |						// Set ACK condition
							1<<TWEN |						// Enable TWI operation
							1<<TWIE;						// Enable TWI interrupt
			}
			break;
		}

		// Data byte has been received; NOT ACK has been returned
		case I2C_STATE_M_RECEIVE_BYTE_NACK: {
			// Store received byte into buffer
			i2c.master.rx_buff[i2c.master.p_buff] = TWDR;
			// Decrement receive queue counter
			i2c.master.q_length--;
			// Set TWI register flags
			TWCR	=	0<<TWSTA |							// Not START
						1<<TWSTO |							// Set STOP condition
						1<<TWINT |							// Flush TWI interrupt flag
						I2C_SLAVE_MODE<<TWEA |				// Enable ACK if SLAVE mode enabled
						1<<TWEN |							// Enable TWI operation
						1<<TWIE;							// Enable TWI interrupt
			// Flush I2C busy flag
			i2c._busy = 0;
			// Flush I2C error flag
			i2c._error = 0;
			// Run normal handling process
			I2C_PROC_MASTER_OUT();
			break;
		}

		#if (I2C_SLAVE_MODE)
		// SLAVE Mode =====================================================================================
		// Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
		case I2C_STATE_SLA_RECEIVE_SLA_W_LP:
		// Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
		case I2C_STATE_SLA_RECEIVE_SLA_W_LP_BC:
		// Set I2C Error
		i2c.error = I2C_ERROR_LOW_PRIORITY;
		// Set flag MASTER was interrupted
		i2c._interrupted = 1;
		// Flush buffers indexes
		i2c.settings.master.pBuff=0;
		i2c.settings.page.pBuff=0;

		// Own SLA+W has been received; ACK has been returned
		case I2C_STATE_SLA_RECEIVE_SLA_W:
		// General call address has been received; ACK has been returned
		case I2C_STATE_SLA_RECEIVE_SLA_W_BC:
		// Set FLAG I2C is busy
		i2c._active = 1;
		// Flush slave buffer index
		i2c.settings.slave.pRXBuff=0;
		// Prepare I2C to receive ->
		if(I2C_SLAVE_LENGTH_RX_BUFFER == 1) {
			//> Receive only one byte of data
			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						0<<TWEA |
						1<<TWEN |
						1<<TWIE;
		} else {
			//> Receive more then one bytes

			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						1<<TWEA |
						1<<TWEN |
						1<<TWIE;
		}
		break;

		// Previously addressed with own SLA+W; data has been received; ACK has been returned
		case I2C_STATE_SLA_RECEIVE_BYTE:
		// Previously addressed with general call; data has been received; ACK has been returned
		case I2C_STATE_SLA_RECEIVE_BYTE_BC:
		// Store received byte into buffer
		i2c.settings.slave.rx_buff[i2c.settings.slave.pRXBuff] = TWDR;
		// Increment buffer index
		i2c.settings.slave.pRXBuff++;
		// If we in the end of buffer
		if(i2c.settings.slave.pRXBuff == (I2C_SLAVE_LENGTH_RX_BUFFER-1)) {
			//> Send NACK
			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						0<<TWEA |
						1<<TWEN |
						1<<TWIE;
		} else {
			//> Send ACK
			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						1<<TWEA |
						1<<TWEN |
						1<<TWIE;
		}
		break;

		// Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
		case I2C_STATE_SLA_RECEIVE_LAST_BYTE:
		// Previously addressed with general call; data has been received; NOT ACK has been returned
		case I2C_STATE_SLA_RECEIVE_LAST_BYTE_BC:
		//
		i2c.settings.slave.rx_buff[i2c.settings.slave.pRXBuff] = TWDR;
		// If MASTER mode was interrupted
		if(i2c._interrupted) {
			//> Send START
			// Set TWI register flags
			TWCR	=	1<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						1<<TWEA |
						1<<TWEN |
						1<<TWIE;
		} else {
			//> Nothing to DO
			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						1<<TWEA |
						1<<TWEN |
						1<<TWIE;
		}
		// Run normal handling process
		I2C_PROC_SLAVE_OUT();
		break;

		// A STOP condition or repeated START condition has been received while still addressed as Slave
		case I2C_STATE_SLA_RECEIVE_RESTART:
		// Set TWI register flags
		TWCR	=	0<<TWSTA |
					0<<TWSTO |
					1<<TWINT |
					1<<TWEA |
					1<<TWEN |
					1<<TWIE;
		break;

		// Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
		case I2C_STATE_SLA_RECEIVE_SLA_R_LP:
		// Set I2C Error
		i2c.error = I2C_ERROR_LOW_PRIORITY;
		// Set flag MASTER was interrupted
		i2c._interrupted = 1;
		// Flush buffers indexes
		i2c.settings.master.pBuff=0;
		i2c.settings.page.pBuff=0;

		// Own SLA+R has been received; ACK has been returned
		case I2C_STATE_SLA_RECEIVE_SLA_R:
		// Flush TX slave buffer index
		i2c.settings.slave.pTXBuff=0;
		// Load data byte from TX buffer to I2C output
		TWDR = i2c.settings.slave.tx_buff[i2c.settings.slave.pTXBuff];
		// If sending only one byte
		if(I2C_SLAVE_LENGTH_TX_BUFFER == 1) {
			//> Waiting NACK state
			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						0<<TWEA |
						1<<TWEN |
						1<<TWIE;
		} else {
			//> Waiting ACK state
			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						1<<TWEA |
						1<<TWEN |
						1<<TWIE;
		}
		break;

		// Data byte in TWDR has been transmitted; ACK has been received
		case I2C_STATE_SLA_SEND_BYTE_RECEIVE_ACK:
		// Increment slave TX buffer index
		i2c.settings.slave.pTXBuff++;
		// Load next byte from slave TX buffer
		TWDR = i2c.settings.slave.tx_buff[i2c.settings.slave.pTXBuff];
		// If sending last byte
		if(i2c.settings.slave.pTXBuff == (I2C_SLAVE_LENGTH_TX_BUFFER-1)) {
			//> Sending NACK
			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						0<<TWEA |
						1<<TWEN |
						1<<TWIE;
		} else {
			//> Sending ACK
			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						1<<TWEA |
						0<<TWEN |
						1<<TWIE;
		}
		break;

		// Data byte in TWDR has been transmitted; NOT ACK has been received
		case I2C_STATE_SLA_SEND_BYTE_RECEIVE_NACK:
		// Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
		case I2C_STATE_SLA_SEND_LAST_BYTE_RECEIVE_ACK:
		// If MASTER mode was interrupted another MASTER
		if(i2c._interrupted) {
			// Flush INTERRUPTED flag
			i2c._interrupted = 0;
			// Set TWI register flags
			TWCR	=	1<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						1<<TWEA |
						1<<TWEN |
						1<<TWIE;
		} else {
			// Set TWI register flags
			TWCR	=	0<<TWSTA |
						0<<TWSTO |
						1<<TWINT |
						1<<TWEA |
						1<<TWEN |
						1<<TWIE;
		}
		// Run normal handling process
		I2C_PROC_SLAVE_OUT();
		break;
		#endif

		default: break;
	}
}


void DoNothing(void)
{
}

void I2C_Init(void)
{
	// Enabling pull up resistors on port pins
	I2C_PORT |= 1<<I2C_PIN_SCL|1<<I2C_PIN_SDA;
	I2C_DDR &= ~(1<<I2C_PIN_SCL|1<<I2C_PIN_SDA);
	// Setup I2C bit rate
	TWBR = I2C_TWBR_VALUE;
	TWSR = I2C_TWSR_VALUE;
}

uint8_t I2C_RequestFrom(uint8_t addr, uint8_t length, uint8_t word_length, uint16_t word_addr)
{
	// Check last request buffer processed
	if(i2c._rx_lock) return 0;
	// Lock I2C to work with our byte
	i2c._busy = 1;
	// Lock RX buffer
	i2c._rx_lock = 1;
	// Preparing page address word by his length
	if(word_length > 0) {
		i2c.transmit_type = I2C_TRANSMIT_TYPE_SAWSARP;
		if(word_length == 2) {
			i2c.page.buff[0] = HI(word_addr);
			i2c.page.buff[1] = LO(word_addr);
		} else {
			i2c.page.buff[0] = LO(word_addr);
		}
		// Flush page word buffer pointer
		i2c.page.p_buff = 0;
		// Set page address word length
		i2c.page.q_length = word_length;
	} else {
		i2c.transmit_type = I2C_TRANSMIT_TYPE_SARP;
	}
	// Setup M41T00S slave I2C address
	i2c.master.slave_addr = addr;
	// Set asynchronous function state
	i2c.master.onSuccess = &DoNothing;
	i2c.master.onError = &DoNothing;
	// Flush buffer pointer - nothing to transmit
	i2c.master.p_buff = 0;
	// Flush read buffer pointer
	i2c.master.p_read = 0;
	// Set RX queue length
	i2c.master.q_length = length;
	// Start transmitter with START
	TWCR = 	1<<TWSTA|						// Set START condition
			0<<TWSTO|						// Not STOP
			1<<TWINT|						// Flush TWI interrupt flag
			I2C_SLAVE_MODE<<TWEA|			// Not ACK
			1<<TWEN|						// Enable TWI operation
			1<<TWIE;						// Enable TWI interrupt
	// Return 1 if buffer is clear
	return 1;
}

void I2C_TransmitTo(uint8_t *data, uint8_t addr, uint8_t length, uint8_t word_length, uint16_t word_addr)
{
	// Store data for write to I2C write buffer
	uint8_t *p_data = (void *)data;
	// Flush write buffer pointer
	i2c.master.p_write=0;
	// Flush queue length
	i2c.master.q_length=0;
	// Prepare data for transmit
	for(uint8_t i=length; i; i--) {
		// Overflow control
		if(i2c.master.p_write >= I2C_M_LENGTH_TX_BUFFER) continue;
		// Store data into TX buffer
		i2c.master.tx_buff[i2c.master.p_write++] = *(p_data++);
	}
	// Lock I2C to work with our byte
	i2c._busy = 1;
	// Set I2C mode to SLA+R mode
	i2c.transmit_type = I2C_TRANSMIT_TYPE_SAWP;
	// Setup M41T00S slave I2C address
	i2c.master.slave_addr = addr;
	// Preparing page address word by his length
	if(word_length == 2) {
		i2c.page.buff[0] = HI(word_addr);
		i2c.page.buff[1] = LO(word_addr);
	} else {
		i2c.page.buff[0] = LO(word_addr);
	}
	// Flush page word buffer pointer
	i2c.page.p_buff = 0;
	// Set page address word length
	i2c.page.q_length = word_length;
	// Set asynchronous function state
	i2c.master.onSuccess = &DoNothing;
	i2c.master.onError = &DoNothing;
	// Flush buffer pointer - nothing to transmit
	i2c.master.p_buff = 0;
	// Set RX queue length
	i2c.master.q_length = length;
	// Start transmitter with START
	TWCR = 	1<<TWSTA|						// Set START condition
			0<<TWSTO|						// Not STOP
			1<<TWINT|						// Flush TWI interrupt flag
			I2C_SLAVE_MODE<<TWEA|			// Not ACK
			1<<TWEN|						// Enable TWI operation
			1<<TWIE;						// Enable TWI interrupt
}

uint8_t I2C_Read(void)
{
	// Flush RX lock flag
	i2c._rx_lock = 0;
	// Overflow control
	if(i2c.master.p_read >= I2C_M_LENGTH_RX_BUFFER) return 0;
	// Return byte of RX buffer
	return i2c.master.rx_buff[i2c.master.p_read++];
}


#if(I2C_SLAVE_MODE)
void I2C_InitSlave(I2C_F onSuccessFunc, I2C_F onErrorFunc)
{
	// Setup SLAVE address for US
	TWAR = I2C_SLAVE_ADDRESS;
	// Set function for data processing
	i2c.settings.slave.actions.onSuccess = onSuccessFunc;
	i2c.settings.slave.actions.onError = onErrorFunc;
	// Set TWI register flags
	TWCR	=	0<<TWSTA |
				0<<TWSTO |
				0<<TWINT |
				1<<TWEA |
				1<<TWEN |
				1<<TWIE;
}
#endif
