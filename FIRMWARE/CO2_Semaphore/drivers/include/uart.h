/*
 * uart.h
 *
 * Created: 23.01.2019 10:57:47
 *  Author: fly8r
 * Version: 2.0 (02.02.2019)
 */
#ifndef DRVUART_H_
#define DRVUART_H_

#include "../../config.h"
#include "../../HAL.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <util/delay.h>


/************************************************************************/
/* MACROS                                                               */
/************************************************************************/
enum UART_ERRORS_ENUM
{
	UART_ERROR_NO_ERROR=0,	
	UART_ERROR_RX_BUFF_OVERFLOW,
	UART_ERROR_RX_FRAME_ERROR
};


/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
/* Pointer to function type */
typedef void (*UART_F)(void);

typedef struct {
	uint8_t						_error;
	enum	UART_ERRORS_ENUM	error;
	struct {
		#if (UART_TX_BUFSIZE > 255)
		uint16_t	p_send,
					p_write,
					q_length;
		#else
		uint8_t		p_send,
					p_write,
					q_length;
		#endif		
		uint8_t		buff[UART_TX_BUFSIZE];
		UART_F		eventComplete,
					eventError;
	} tx;
	struct {
		#if (UART_RX_BUFSIZE > 255)
		uint16_t	p_read,
					q_length,
					data_length;
		#else
		uint8_t		p_read,
					q_length,
					data_length;
		#endif
		uint8_t		buff[UART_RX_BUFSIZE];
		UART_F		eventRxComplete,
					eventError;
	} rx;
} uart_data_t;
extern	uart_data_t	uart;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void UART_AddString(const char * s);
void UART_AddStringFromFlash(const char * s);
#if (UART_TX_BUFSIZE > 255)
void UART_SendData(char *data, uint16_t data_length);
void UART_SendDataFromFlash(const char *data, uint16_t data_length);
void UART_DefferedReadData(uint16_t length, UART_F complete_handler, UART_F error_handler);
#else
void UART_SendData(char *data, uint8_t data_length);
void UART_SendDataFromFlash(const char *data, uint8_t data_length);
void UART_DefferedReadData(uint8_t length, UART_F complete_handler, UART_F error_handler);
#endif
uint8_t UART_Read();

void UART_UDRE_TX_Service(void);
void UART_RX_Service(void);
void UART_RX_FlushBuffer(void);
void UART_DoNothing(void);


#endif /* DRVUART_H_ */
