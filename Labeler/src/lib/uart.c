
/*
 * uart.c
 *
 * Created: 03.06.2022 10:49:37
 *  Author: ruwen
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

void uart_send_32bit(uint32_t data)
{
	// Obere beiden Bytes werden ausgeblendet, untere werden gesendet
	uart_send_16bit(data & 0xffff);
	// Obere Bytes werden auf untere Postion verschoben, füllt obere mit Nullen
	// Ausblenden der neuen oberen Bytes zur Sicherheit
	uart_send_16bit((data >> 16) & 0xffff);
}

unsigned char uart_rec(void)
{
	while(!(UCSR0A & (1<<RXC0)));
	
	return UDR0;
}


//______________________________________
// Zum Testen


unsigned char uart_rec_u(void)
{
	while(!(UCSR0A & (1<<RXC0)));
	
	return UDR0;
	
}

uint8_t uart_get(void)
{
	while(!(UCSR0A & (1<<RXC0)));
	
	return UDR0;
	
}


void uart_gets( char* Buffer, uint8_t MaxLen )
{
	uint8_t NextChar;
	uint8_t StringLen = 0;

	NextChar = uart_get();         // Warte auf und empfange das nächste Zeichen

	// Sammle solange Zeichen, bis:
	// * entweder das String Ende Zeichen kam
	// * oder das aufnehmende Array voll ist
	while( NextChar != '\0' && StringLen < MaxLen - 1 ) {
		*Buffer++ = NextChar;
		StringLen++;
		NextChar = uart_get();
	}

	// Noch ein '\0' anhängen um einen Standard
	// C-String daraus zu machen
	*Buffer = '\0';
}







int uart_putc(unsigned char c)
{
	while (!(UCSR0A & (1<<UDRE0)))  /* warten bis Senden moeglich */

	UDR0 = c;                      /* sende Zeichen */
	return 0;
}


/* puts ist unabhaengig vom Controllertyp */
void uart_puts (char *s)
{
	while (*s)
	{   /* so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)" */
		uart_putc(*s);
		s++;
	}
}