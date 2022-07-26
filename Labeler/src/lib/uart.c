
/*
 * uart.c
 *
 * Created: 03.06.2022 10:49:37
 *  Author: Oli 
 */ 


#include "uart.h"


// Es funktioniert nur mit UART0 (von 4) keine Ahnung warum
void uart_init(void)
{
	// Baudrate konfigurieren, siehe constants.h
	UBRR0H = (uint8_t) (BAUD_UBRR >> 8);
	UBRR0L = (uint8_t) (BAUD_UBRR & 0x0ff);
	
	// Senden und Empfangen einschalten
	UCSR0B =   (1<<TXEN0) | (1<<RXEN0);
	
	// 8 Datenbits, 1 Stoppbit
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void uart_send(unsigned char data)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void uart_send_16bit(uint16_t data)
{
	// Oberes Byte wird ausgeblendet, unteres wird gesendet
	uart_send(data & 0xff);
	// Oberes Byte wird auf untere Postion verschoben, füllt oberes mit Nullen
	// Ausblenden des neuen oberen Bytes zur Sicherheit
	uart_send((data >> 8) & 0xff);
}



unsigned char uart_rec(void)
{
	while(!(UCSR0A & (1<<RXC0)));
	
	return UDR0;
}

// UART-ISR um Schlittenpositionen zu senden
void uart_interrupt_init(void)
{
	// Alle 16ms bei prescaler 1024;
	OCR0A = 249;
	
	// Clear COM0A1 + COM0A0 for disconnecting OC0A. Usable for Normal port operation (p. 126)
	TCCR0A &= ~((1<<COM0A1) | (1<<COM0A0));
	
	TCCR0A |= (1<<WGM01);
	
	TCCR0B = (1<<CS02) | (1<<CS00);
	
	// Enable Timer-ISR
	TIMSK0 |= (1<<OCIE0A);
	
}

// Pollt regelmäßig die UART-Nutzereingabe (HTerm), bis der Buffer (32 Zeichen) voll ist 
// oder ein Nullterminator registriert wird. Baudrate sollte nicht zu hoch gewählt sein, da wir ansonsten HTerm verpassen
void UserInputHandler_poll_input(UserInputHandler* handler)
{
	if (handler->status == INPUT_INCOMPLETED && handler->bufidx < 32)
	{
		// Ist was im Buffer?
		if (UCSR0A & (1 << RXC0))
		{
			char c = UDR0;
			
			// HTerm lässt uns kein \n als Ascii senden. Deswegen muss '#' ausreichen.
			// Füllen die restliche Zeile mit Leerzeichen auf, damit das LCD es easy anzeigen kann
			if (c == '#' && handler->bufidx <= 15)
			{
				handler->second_row_at = handler->bufidx;
				uint8_t fill = 16-handler->bufidx;
			
				// Pad restliche Eingabe mit Leerzeichen
				for (uint8_t i = 0; i < fill; i++)
				{
					handler->input_buffer[handler->bufidx+i] = ' ';
				}
				handler->bufidx += fill;
			}

			// Puffer die Nutzereingabe
			else
			{
				handler->input_buffer[handler->bufidx] = c;
				++handler->bufidx;
			}
			
			// Nullterminator wird registriert. Eingabe somit komplett
			if (c == '\0')
			{
				handler->status = INPUT_COMPLETED;
			}
			
			// Eingabe überschreitet 32 Buchstaben. Füge Nullterminator hinzu und Eingabe ist somit komplett
			else if (handler->bufidx == 32)
			{
				handler->input_buffer[handler->bufidx] = '\0';
				handler->status = INPUT_COMPLETED;
			}
		}
	}
}

void UserInputHandler_reset_input(UserInputHandler* handler)
{
	handler->bufidx = 0;
	handler->second_row_at = 16;
	handler->status = INPUT_INCOMPLETED;
}

