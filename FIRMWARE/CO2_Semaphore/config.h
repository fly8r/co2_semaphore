/*
 * config.h
 *
 * Created: 23.01.2019 10:39:34
 *  Author: fly8r
 */
#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU							8000000UL	// Core CPU Frequency

//----------------------------- Initialize default date/time
#define DEFAULT_DATE_DAY				01
#define DEFAULT_DATE_MONTH				02
#define DEFAULT_DATE_YEAR				19
#define DEFAULT_DATE_DOW				4
#define DEFAULT_TIME_HOUR				14
#define DEFAULT_TIME_MINUTE				52

//----------------------------- UART params
#define BAUDRATE						9600L	// UART configuration
#define UART_RX_BUFSIZE					10		// RX buffer size
#define UART_TX_BUFSIZE					20		// TX buffer size


#endif