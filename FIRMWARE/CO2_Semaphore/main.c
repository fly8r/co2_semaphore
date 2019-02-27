/*
 * CO2_Semaphore.c
 *
 * Created: 21.01.2019 13:48:21
 * Author : fly8r
 */
#include "main.h"

/* Functions prototypes */
void InitGPIO(void);

//------------------------------ System tick interrupt
ISR(TIMER1_COMPA_vect)
{
	systick++;
}

//------------------------------ INT0 interrupt
ISR(INT0_vect)
{
	// Toggle external rtc clock
	device._ext_clock ^= 0x1;
}

//------------------------------ USART RX Interrupt vector
ISR(USART_RX_vect)
{
	UART_RX_Service();
}

//------------------------------ USART UDRE Interrupt vector
ISR(USART_UDRE_vect)
{
	UART_UDRE_TX_Service();
}

//------------------------------ TWI interrupt
ISR(TWI_vect)
{
	I2C_TWIService();
}

//------------------------------ Entry point
int main(void)
{
	// Main initialize
	InitGPIO();
	InitMessages();
	InitTimers();
	I2C_Init();
	// FSMs initialize
	FSM_SYSTEM_Init();
	FSM_UART_Init();
	FSM_PCF8574_Init();
	FSM_LCD_Init();
	FSM_X1226_Init();
	FSM_MHZ19B_Init();
	FSM_DHT_Init();
	FSM_BUTTON_Init();
	FSM_ENC_Init();
	FSM_LEDS_Init();
	FSM_BUZZER_Init();

	// Enable global interrupts
	sei();

	// Main loop
    while (1) {
		// Processing software timers queue
		ProcessTimers(&systick);
		// Processing FSMs
		FSM_SYSTEM_Process();
		FSM_UART_Process();
		FSM_PCF8574_Process();
		FSM_LCD_Process();
		FSM_X1226_Process();
		FSM_DHT_Process();
		FSM_MHZ19B_Process();
		FSM_BUTTON_Process();
		FSM_ENC_Process();
		FSM_LEDS_Process();
		FSM_BUZZER_Process();

		// Processing messages queue
		ProcessMessages();
    }
}

//------------------------------ MCU IO initialization
inline void InitGPIO(void)
{
	// Power reduction initialize
	POWER_REDUCTION_INIT();
	// Initialize systick timer
	SYSTICK_TIMER_INIT();
	SYSTICK_INTERRUPT_ENABLE();
	// Initialize timer for measurement
	MEASURING_TMR_INIT();
	// Initialize UART
	UART_INIT();
	UART_TX_INTERRUPT_ENABLE();
	// Initialize RTC IRQ input INT0
	X1226_IRQ_INIT();
	// Initialization DHT sensor port
	DHT_INIT();
	// Initialize button port
	BTN_INIT();
	// Initialize encoder port
	ENC_INIT();
	// Initialize red led port
	LED_RED_INIT();
	// Initialize yellow port
	LED_YELLOW_INIT();
	// Initialize green led port
	LED_GRN_INIT();
	// Initialize LCD back light control port
	BL_CTRL_INIT();
	// Initialize BUZZER port
	BUZZER_INIT();
}

