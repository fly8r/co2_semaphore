/*
 * uart.c
 *
 * Created: 23.01.2019 11:00:28
 *  Author: fly8r
 */
#include "include/uart.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
uart_data_t		uart;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
uint8_t UART_RX_FlushUDR(void);

/* Add string to UART TX cycle buffer */
void UART_AddString(const char *s)
{
	#if (UART_TX_BUFSIZE > 255)
	uint16_t	data_length=0,
				q_length=0;
	#else
	uint8_t		data_length=0,
				q_length=0;
	#endif

	// Calculating string length with buffer overload check
	while(*(s++)) {
		// Trim data more then available buff size
		if(data_length++ >= UART_TX_BUFSIZE) break;
	}

	// Disable UART UDRE Interrupt
	UART_TX_INTERRUPT_DISABLE();
	q_length = uart.tx.q_length;
	// Enable UART_UDRE Interrupt
	UART_TX_INTERRUPT_ENABLE();

	// If buffer have enough free space for sending string - load string into TX buffer
	if(data_length < (UART_TX_BUFSIZE - q_length)) {
		while(data_length--) {
			// Load char from string into TX buffer with offset
			uart.tx.buff[uart.tx.p_write++] = *(s++);
			// Detect loop back in TX buffer offset
			if(uart.tx.p_write >= UART_TX_BUFSIZE) uart.tx.p_write=0;
			UART_TX_INTERRUPT_DISABLE();
			// Increment TX queue counter length
			uart.tx.q_length++;
			UART_TX_INTERRUPT_ENABLE();
		}
	}
}

#if (UART_TX_BUFSIZE > 255)
void UART_SendData(char *data, uint16_t data_length)
#else
void UART_SendData(char *data, uint8_t data_length)
#endif
{
	#if (UART_TX_BUFSIZE > 255)
	uint16_t	q_length=0;
	#else
	uint8_t		q_length=0;
	#endif

	// Disable UART UDRE Interrupt
	UART_TX_INTERRUPT_DISABLE();
	q_length = uart.tx.q_length;
	// Enable UART_UDRE Interrupt
	UART_TX_INTERRUPT_ENABLE();

	if(data_length < (UART_TX_BUFSIZE - q_length)) {
		while(data_length--) {
			// Load char from string into TX buffer with offset
			uart.tx.buff[uart.tx.p_write++] = *(data++);
			// Detect loop back in TX buffer offset
			if(uart.tx.p_write >= UART_TX_BUFSIZE) uart.tx.p_write=0;
			UART_TX_INTERRUPT_DISABLE();
			// Increment TX queue counter length
			uart.tx.q_length++;
			UART_TX_INTERRUPT_ENABLE();
		}
	}
}


/* Add to UART TX buffer string from FLASH memory */
void UART_AddStringFromFlash(const char *s)
{
	#if (UART_TX_BUFSIZE > 255)
	uint16_t	data_length=0,
				q_length=0;
	#else
	uint8_t		data_length=0,
				q_length=0;
	#endif

	// Calculating string length with buffer overload check
	while(pgm_read_byte(s++)) {
		if(data_length++ >= UART_TX_BUFSIZE) break;
	}

	// Disable UART UDRE Interrupt
	UART_TX_INTERRUPT_DISABLE();
	q_length = uart.tx.q_length;
	// Enable UART_UDRE Interrupt
	UART_TX_INTERRUPT_ENABLE();

	// If buffer have enough free space for sending string - load string into TX buffer
	if(data_length < (UART_TX_BUFSIZE - q_length)) {
		while(data_length--) {
			// Load char from FLASH string into TX buffer with offset
			uart.tx.buff[uart.tx.p_write++] = pgm_read_byte(s++);
			// Detect loop back in TX buffer offset
			if(uart.tx.p_write >= UART_TX_BUFSIZE) uart.tx.p_write=0;
			// Disable UART UDRE Interrupt
			UART_TX_INTERRUPT_DISABLE();
			// Increment TX queue counter length
			uart.tx.q_length++;
			// Enable UART_UDRE Interrupt
			UART_TX_INTERRUPT_ENABLE();
		}
	}
}

#if (UART_TX_BUFSIZE > 255)
void UART_SendDataFromFlash(const char *data, uint16_t data_length)
#else
void UART_SendDataFromFlash(const char *data, uint8_t data_length)
#endif
{
	#if (UART_TX_BUFSIZE > 255)
	uint16_t	q_length=0;
	#else
	uint8_t		q_length=0;
	#endif

	// Disable UART UDRE Interrupt
	UART_TX_INTERRUPT_DISABLE();
	q_length = uart.tx.q_length;
	// Enable UART_UDRE Interrupt
	UART_TX_INTERRUPT_ENABLE();

	if(data_length <= (UART_TX_BUFSIZE - q_length)) {
		while(data_length--) {
			// Load char from FLASH string into TX buffer with offset
			uart.tx.buff[uart.tx.p_write++] = pgm_read_byte(data++);
			// Detect loop back in TX buffer offset
			if(uart.tx.p_write >= UART_TX_BUFSIZE) uart.tx.p_write=0;
			UART_TX_INTERRUPT_DISABLE();
			// Increment TX queue counter length
			uart.tx.q_length++;
			UART_TX_INTERRUPT_ENABLE();
		}
	}
}

/* ISR Service for UDRE interrupt */
void UART_UDRE_TX_Service(void)
{
	if(uart.tx.q_length) {
		// Copy char from TX buffer to UDR register
		UDR0 = uart.tx.buff[uart.tx.p_send++];
		// Detect loop back in TX sent offset
		if(uart.tx.p_send >= UART_TX_BUFSIZE) uart.tx.p_send=0;
		// Decrement TX queue
		uart.tx.q_length--;
	} else {
		UART_TX_INTERRUPT_DISABLE();
	}
}

/* ISR Service for RX interrupt */
void UART_RX_Service(void)
{
	// Read UART data
	int8_t	data = UDR0;

	if(!uart.rx.data_length) {
		switch(data) {
			// ENTER key was received
			case '\r': {
				(uart.rx.eventRxComplete)();
				return;
			}

			#if (UART_RX_ECHO)
			case 127:
			case 0x8: {
				// If not first char in string
				if(uart.rx.q_length) {
					// Echo data into UART
					UDR0 = data;
					// Decrement buffer pointer
					uart.rx.q_length--;
					// Decrement buffer pointer
					if(!uart.rx.p_read) {
						uart.rx.p_read = UART_RX_BUFSIZE;
					}
					uart.rx.p_read--;
				}
				return;
			}
			#endif

			default: break;
		}
	}
	// If buffer not full - processing
	if(uart.rx.q_length <= UART_RX_BUFSIZE) {
		// Echo data into UART
		#if (UART_RX_ECHO)
		UDR0 = data;
		#endif
		// Add data to RX buffer
		uart.rx.buff[uart.rx.p_read++] = data;
		// If getting tail buffer, then point to start buffer
		if(uart.rx.p_read == UART_RX_BUFSIZE) uart.rx.p_read = 0;
		// Increment data counter
		uart.rx.q_length++;
		// If we received all data bytes - call event function
		if(uart.rx.data_length && uart.rx.q_length == uart.rx.data_length) {
			// Disable RX interrupt
			UART_RX_INTERRUPT_DISABLE();
			// Reset buffer pointer
			uart.rx.p_read=0;
			// Call event procedure
			(uart.rx.eventRxComplete)();
		}
	} else {
		// Set error flag
		uart._error = 1;
		// Set error identificator
		uart.error = UART_ERROR_RX_BUFF_OVERFLOW;
		// Disable RX interrupt
		UART_RX_INTERRUPT_DISABLE();
		// Call error handler
		(uart.rx.eventError)();
	}
}

uint8_t UART_Read()
{
	// Check buffer overflow
	if(uart.rx.p_read > UART_RX_BUFSIZE) uart.rx.p_read=0;
	// Read buffer value
	return uart.rx.buff[uart.rx.p_read++];
}

#if (UART_TX_BUFSIZE > 255)
void UART_DefferedReadData(uint16_t length, UART_F complete_handler, UART_F error_handler)
#else
void UART_DefferedReadData(uint8_t length, UART_F complete_handler, UART_F error_handler)
#endif
{
	// Reset buffer pointer
	uart.rx.p_read=0;
	// Reset queue length data
	uart.rx.q_length=0;
	// Set data length
	uart.rx.data_length = length;
	// Set event procedure
	uart.rx.eventRxComplete = complete_handler;
	// Set event ERROR
	uart.rx.eventError = error_handler;
	// Enable RX interrupt
	UART_RX_INTERRUPT_ENABLE();
}

/* Flush UART buffer */
void UART_RX_FlushBuffer(void)
{
	uart.rx.p_read=0;
	uart.rx.q_length=0;
}

uint8_t UART_RX_FlushUDR(void)
{
	volatile uint8_t dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
	return dummy;
}


void UART_DoNothing(void)
{
}