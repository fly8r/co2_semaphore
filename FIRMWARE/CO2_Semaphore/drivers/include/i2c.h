/*
 * i2c.h
 *
 * Created: 23.01.2019 10:50:11
 *  Author: fly8r
 * Version: 1.1 (25.01.2019)
 */
#ifndef DRVI2C_H_
#define DRVI2C_H_


#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../../config.h"
#include "../../HAL.h"

//------------------------------ I2C states
//> MASTER mode
#define I2C_STATE_M_BUS_FAIL						0x00				// Bus error due to an illegal START or STOP condition
#define I2C_STATE_M_START							0x08				// A START condition has been transmitted
#define I2C_STATE_M_RESTART							0x10				// A repeated START condition has been transmitted
#define I2C_STATE_M_SLA_W_ACK						0x18				// SLA+W has been transmitted; ACK has been received
#define I2C_STATE_M_SLA_W_NACK						0x20				// SLA+W has been transmitted; NOT ACK has been received
#define I2C_STATE_M_BYTE_ACK						0x28				// Data byte has been transmitted; ACK has been received
#define I2C_STATE_M_BYTE_NACK						0x30				// Data byte has been transmitted; NOT ACK has been received
#define I2C_STATE_M_COLLISION						0x38				// Arbitration lost in SLA+W or data bytes
#define I2C_STATE_M_SLA_R_ACK						0x40				// SLA+R has been transmitted; ACK has been received
#define I2C_STATE_M_SLA_R_NACK						0x48				// SLA+R has been transmitted; NOT ACK has been received
#define I2C_STATE_M_RECEIVE_BYTE					0x50				// Data byte has been received; ACK has been returned
#define I2C_STATE_M_RECEIVE_BYTE_NACK				0x58				// Data byte has been received; NOT ACK has been returned

//> SLAVE mode
#define I2C_STATE_SLA_RECEIVE_SLA_W_LP				0x68				// Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
#define I2C_STATE_SLA_RECEIVE_SLA_W_LP_BC			0x78				// Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
#define I2C_STATE_SLA_RECEIVE_SLA_W					0x60				// Own SLA+W has been received; ACK has been returned
#define I2C_STATE_SLA_RECEIVE_SLA_W_BC				0x70				// General call address has been received; ACK has been returned
#define I2C_STATE_SLA_RECEIVE_BYTE					0x80				// Previously addressed with own SLA+W; data has been received; ACK has been returned
#define I2C_STATE_SLA_RECEIVE_BYTE_BC				0x90				// Previously addressed with general call; data has been received; ACK has been returned
#define I2C_STATE_SLA_RECEIVE_LAST_BYTE				0x88				// Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
#define I2C_STATE_SLA_RECEIVE_LAST_BYTE_BC			0x98				// Previously addressed with general call; data has been received; NOT ACK has been returned
#define I2C_STATE_SLA_RECEIVE_RESTART				0xA0				// A STOP condition or repeated START condition has been received while still addressed as Slave
#define I2C_STATE_SLA_RECEIVE_SLA_R_LP				0xB0				// Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
#define I2C_STATE_SLA_RECEIVE_SLA_R					0xA8				// Own SLA+R has been received; ACK has been returned
#define I2C_STATE_SLA_SEND_BYTE_RECEIVE_ACK			0xB8				// Data byte in TWDR has been transmitted; ACK has been received
#define I2C_STATE_SLA_SEND_BYTE_RECEIVE_NACK		0xC0				// Data byte in TWDR has been transmitted; NOT ACK has been received
#define I2C_STATE_SLA_SEND_LAST_BYTE_RECEIVE_ACK	0xC8				// Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received

//
#define I2C_PROC_MASTER_OUT()			            (i2c.master.onSuccess)()
#define I2C_PROC_SLAVE_OUT()			            (i2c.settings.slave.actions.onSuccess)()
#define I2C_PROC_ERROR_OUT()			            (i2c.master.onError)()

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
/* Pointer to function type */
typedef void (*I2C_F)(void);

/* I2C Possible errors */
enum I2C_ERRORS_ENUM
{
	I2C_ERROR_NO,							// No errors
	I2C_ERROR_DEVICE_NOT_FOUND,				// Device no answer
	I2C_ERROR_LOW_PRIORITY,					// Low priority
	I2C_ERROR_RECEIVED_NACK,				// Received NACK. End transmission
	I2C_ERROR_BUS_FAIL						// Bus fail
};

/* I2C Transmitter mode */
enum I2C_TRANSMIT_TYPE_ENUM
{
	I2C_TRANSMIT_TYPE_SARP,					// Start-Addr_r-Read-Stop	// Simple read mode from current address
	I2C_TRANSMIT_TYPE_SAWP,					// Start-Addr_w-Write-Stop	// Simple write mode
	I2C_TRANSMIT_TYPE_SAWSARP				// Start-Addr_w-WrPageAddr-rStart-Addr_R-Read-Stop // Read mode with preparing page address
};

typedef struct
{
	uint8_t		_busy, _rx_lock, _error, _interrupted;
	enum		I2C_TRANSMIT_TYPE_ENUM		transmit_type;
	enum		I2C_ERRORS_ENUM				error;

	struct {
		uint8_t			slave_addr,
						p_buff,
						p_write,
						p_read,
						q_length;
		uint8_t			tx_buff[I2C_M_LENGTH_TX_BUFFER],
						rx_buff[I2C_M_LENGTH_RX_BUFFER];
		I2C_F			onSuccess,
						onError;
	} master;

	struct {
		uint8_t			p_buff,
		q_length;
		uint8_t			buff[I2C_PAGE_WORD_LENGTH];
	} page;

	struct {

		#if(I2C_SLAVE_MODE)
		// For SLAVE mode
		struct {
			uint8_t		tx_buff[I2C_SLAVE_LENGTH_TX_BUFFER],
						rx_buff[I2C_SLAVE_LENGTH_RX_BUFFER],
						pTXBuff,
						pRXBuff,
						tx_queue_length,
						rx_queue_length;
			struct  {
				I2C_F	onSuccess,
				onError;
			} actions;
		} slave;
		#endif
	} settings;

} i2c_data_t;
extern		i2c_data_t		i2c;


/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void        I2C_TWIService(void);
void        I2C_Init(void);
void		DoNothing(void);
uint8_t		I2C_RequestFrom(uint8_t addr, uint8_t length, uint8_t word_length, uint16_t word_addr);
void		I2C_TransmitTo(uint8_t *data, uint8_t addr, uint8_t length, uint8_t word_length, uint16_t word_addr);
uint8_t		I2C_Read(void);
void		I2C_PrepareWrite(void);
void		I2C_Write(uint8_t data);
#if(I2C_SLAVE_MODE)
void        I2C_InitSlave(I2C_F onSuccessFunc, I2C_F onErrorFunc);
#endif


#endif /* DRVI2C_H_ */