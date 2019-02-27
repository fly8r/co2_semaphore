/*
 * HAL.h
 *
 * Created: 23.01.2019 10:41:33
 *  Author: fly8r
 */
#ifndef HAL_H_
#define HAL_H_

#include <avr/io.h>

//------------------------------ Macros
#define HI(x)							((x)>>8)
#define LO(x)							((x)& 0xFF)
#define STATE_ON						1
#define STATE_OFF						0

//------------------------------ Power management for Mega328P
#define POWER_REDUCTION_INIT()			{ PRR=(1<<PRSPI|PRADC); }

//------------------------------ System timer configuration for 1mS interrupt
#define SYSTICK_TIME_MS					1
#define SYSTICK_PRESCALER				64UL
#define SYSTICK_OCR_CONST				((F_CPU*SYSTICK_TIME_MS) / (SYSTICK_PRESCALER*1000))
#define SYSTICK_TIMER_COUNTER           TCNT1
#define SYSTICK_TIMER_OCR               OCR1A
#define SYSTICK_TIMER_INIT()            { TCCR1A=0; TCCR1B=(1<<WGM12|1<<CS11|1<<CS10); SYSTICK_TIMER_OCR=SYSTICK_OCR_CONST; SYSTICK_TIMER_COUNTER=0; }
#define SYSTICK_INTERRUPT_ENABLE()      { TIMSK1 |= 1<<OCIE1A; }
#define SYSTICK_INTERRUPT_DISABLE()     { TIMSK1 &= ~(1<<OCIE1A); }

//------------------------------ Measurement timer configuration
#define MEASURING_TMR_CNT				TCNT2
#define MEASURING_TMR_INIT()			{ ASSR=0; TCCR2A=0; TCCR2B=(1<<CS21); TIMSK2&=~(1<<TOIE2); MEASURING_TMR_CNT=0; }		// 1us

//------------------------------ UART configuration
#define UBRR_CONST						((F_CPU/(BAUDRATE * 16)) - 1)
#define UART_INIT()						{ UBRR0H=HI(UBRR_CONST); UBRR0L=LO(UBRR_CONST); UCSR0B=(1<<TXEN0); UCSR0C=(1<<UCSZ01|1<<UCSZ00); }
#define UART_TX_INTERRUPT_ENABLE()		{ UCSR0B |= (1<<UDRIE0); }
#define UART_TX_INTERRUPT_DISABLE()		{ UCSR0B &= ~(1<<UDRIE0); }
#define UART_RX_INTERRUPT_ENABLE()		{ UCSR0B |= (1<<RXEN0|1<<RXCIE0); }
#define UART_RX_INTERRUPT_DISABLE()		{ UCSR0B &= ~(1<<RXEN0|1<<RXCIE0); }
#define UART_RX_ECHO					0		// 1 - enabled, 0 - disabled

//------------------------------ I2C configuration
#define I2C_DDR							DDRC
#define I2C_PORT						PORTC
#define I2C_PIN_REG						PINC
#define I2C_PIN_SCL						5
#define I2C_PIN_SDA						4
// I2C Frequency settings
#define I2C_SCL_FREQ                    300000
#define I2C_FREQ_PRESCALER              1
#define I2C_TWSR_VALUE                  0
#define I2C_TWBR_VALUE                  ((F_CPU - 16*I2C_SCL_FREQ) / (2*I2C_SCL_FREQ*I2C_FREQ_PRESCALER))
// I2C mode options
#define I2C_M_LENGTH_TX_BUFFER			20						// TX buffer length in MASTER mode
#define I2C_M_LENGTH_RX_BUFFER			10						// RX buffer length in MASTER mode
#define I2C_PAGE_WORD_LENGTH			2						// Page address word length in bytes
#define I2C_SLAVE_ADDRESS				0x32					// I2C address in SLAVE mode. The last bit activate broadcast
#define I2C_SLAVE_MODE					0						// Enable slave mode
#define I2C_SLAVE_LENGTH_RX_BUFFER		1						// RX buffer length in SLAVE mode
#define I2C_SLAVE_LENGTH_TX_BUFFER		1						// TX buffer length in SLAVE mode

//------------------------------ Display configuration
#define PCF8574_ROWS					4
#define PCF8574_COLS					20
#define PCF8574_QUEUE_SIZE				(PCF8574_COLS * PCF8574_ROWS * 2)
//> IC configuration
#define PCF8574_ADDRESS                 0x40 //0b10000000       // 0x4E is default PCF8574 device address at I2C
#define PCF8574_BL_CTRL					0						// Use for control back light

//------------------------------ PCF8574 LCD pin out configuration
#define PCF8574_PIN_BL_MASK             (1<<3)
#define PCF8574_PIN_E_MASK              (1<<1)
#define PCF8574_PIN_RW_MASK             (1<<2)
#define PCF8574_PIN_RS_MASK             (1<<0)
#define PCF8574_USE_CIRILLIC_TABLE		0

//------------------------------ LCD Back light control
#define BL_CTRL_DDR						DDRD
#define BL_CTRL_PORT					PORTD
#define BL_CTRL_MASK					(1<<5)
#define BL_CTRL_ON()					{ BL_CTRL_PORT |= BL_CTRL_MASK; }
#define BL_CTRL_OFF()					{ BL_CTRL_PORT &= ~BL_CTRL_MASK; }
#define BL_CTRL_INIT()					{ TCCR0A=(1<<WGM01|1<<WGM00|1<<COM0B1); TCCR0B=(1<<CS01|1<<CS00); OCR0B=55; BL_CTRL_DDR|=BL_CTRL_MASK; }

//------------------------------ RTC configuration
#define X1226_CCR_ADDRESS				0b11011110				// CCR memory access address
#define X1226_EEPROM_ADDRESS			0b10101110				// EEPROM memory access address
#define X1226_IRQ_DDR					DDRD
#define X1226_IRQ_PIN					PIND
#define X1226_IRQ_MASK					(1<<2)
#define X1226_IRQ_INIT()				{ X1226_IRQ_DDR &= ~(X1226_IRQ_MASK); EICRA = (1<<ISC01); EIMSK = (1<<INT0); }

//------------------------------ Encoder configuration
#define ENC_DDR							DDRC
#define ENC_PIN							PINC
#define ENC_MASK						(1<<0|1<<1)
#define ENC_INIT()						{ ENC_DDR &= ~(ENC_MASK); }
#define ENC_STATE						(ENC_PIN & ENC_MASK)

//------------------------------ Encoder button configuration
#define BTN_DDR							DDRC
#define BTN_PIN							PINC
#define BTN_MASK						(1<<2)
#define BTN_INIT()						{ BTN_DDR &= ~(BTN_MASK); }
#define BTN_PRESSED						(!(BTN_PIN & BTN_MASK))

//------------------------------ DHT configuration
#define DHT_DDR							DDRD
#define DHT_PORT						PORTD
#define DHT_PIN							PIND
#define DHT_PIN_MASK					(1<<6)
#define DHT_INIT()						{ ACSR = (1<<ACD); } //<- Analog comparator must be OFF
#define DHT_PIN_STATE					(DHT_PIN & DHT_PIN_MASK)

//------------------------------ Configuration IO for GREEN LED
#define LED_GRN_DDR						DDRB
#define LED_GRN_PORT					PORTB
#define LED_GRN_MASK					(1<<0)
#define LED_GRN_ON()					{ LED_GRN_PORT |= LED_GRN_MASK; }
#define LED_GRN_OFF()					{ LED_GRN_PORT &= ~LED_GRN_MASK; }
#define LED_GRN_TOGGLE()				{ LED_GRN_PORT ^= LED_GRN_MASK; }
#define LED_GRN_INIT()					{ LED_GRN_DDR |= LED_GRN_MASK; LED_GRN_OFF(); }

//------------------------------ Configuration IO for YELLOW LED
#define LED_YELLOW_DDR					DDRB
#define LED_YELLOW_PORT					PORTB
#define LED_YELLOW_MASK					(1<<1)
#define LED_YELLOW_ON()					{ LED_YELLOW_PORT |= LED_YELLOW_MASK; }
#define LED_YELLOW_OFF()				{ LED_YELLOW_PORT &= ~LED_YELLOW_MASK; }
#define LED_YELLOW_TOGGLE()				{ LED_YELLOW_PORT ^= LED_YELLOW_MASK; }
#define LED_YELLOW_INIT()				{ LED_YELLOW_DDR |= LED_YELLOW_MASK; LED_YELLOW_OFF(); }

//------------------------------ Configuration IO for RED LED
#define LED_RED_DDR						DDRB
#define LED_RED_PORT					PORTB
#define LED_RED_MASK					(1<<2)
#define LED_RED_ON()					{ LED_RED_PORT |= LED_RED_MASK; }
#define LED_RED_OFF()					{ LED_RED_PORT &= ~LED_RED_MASK; }
#define LED_RED_TOGGLE()				{ LED_RED_PORT ^= LED_RED_MASK; }
#define LED_RED_INIT()					{ LED_RED_DDR |= LED_RED_MASK; LED_RED_OFF(); }

//------------------------------ Configuration IO for BUZZER
#define BUZZER_DDR						DDRD
#define BUZZER_PORT						PORTD
#define BUZZER_MASK						(1<<7)
#define BUZZER_ON()						{ BUZZER_PORT |= BUZZER_MASK; }
#define BUZZER_OFF()					{ BUZZER_PORT &= ~BUZZER_MASK; }
#define BUZZER_TOGGLE()					{ BUZZER_PORT ^= BUZZER_MASK; }
#define BUZZER_INIT()					{ BUZZER_DDR |= BUZZER_MASK; BUZZER_OFF(); }

#endif /* HAL_H_ */