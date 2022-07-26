/*
 * uart.h
 *
 * Created: 24.05.2022 10:49:28
 *  Author: Oli 
 */ 


#ifndef UART_H_
#define UART_H_

#include "constants.h"
#include <avr/interrupt.h>



// Es funktioniert nur mit UART0 (von 4) keine Ahnung warum
void uart_init(void);

void uart_send(unsigned char data);

void uart_send_16bit(uint16_t data);

unsigned char uart_rec(void);

void uart_interrupt_init(void);


typedef enum
{
	INPUT_INCOMPLETED,
	INPUT_COMPLETED
} UserInputHandler_Status;

// Greift sich iterativ Buchstaben aus dem Sendebuffer, bis die Eingabe komplett ist
typedef struct
{
	uint8_t bufidx;
	uint8_t second_row_at;
	UserInputHandler_Status status;
	char input_buffer[33];
} UserInputHandler;


void UserInputHandler_poll_input(UserInputHandler* handler);

void UserInputHandler_reset_input(UserInputHandler* handler);


#define RESET_INPUT_BUF \
for (uint8_t i = 0; i < 33; i++) \
{ \
	input_handler.input_buffer[i] = 0; \
} \

#define EMPTY_UART \
while (UCSR0A & (1 << RXC0)) \
{ \
	char c = UDR0; \
} \

#endif /* UART_H_ */