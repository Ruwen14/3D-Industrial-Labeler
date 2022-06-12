/*
 * Labeler.c
 *
 * Created: 18.05.2022 12:12:03
 * Author : ruwen
 */ 


#include "lib/movement.h"
#include "lib/constants.h"
#include "lib/uart.h"
#include "lib/limit_switches.h"
#include "lib/ZAxisController.h"
#include "lib/XYAxisController.h"
#include "lib/ADCLaser.h"
#include "lib/LCD.h"
#include <avr/interrupt.h>

volatile uint8_t limit_x_left = 0;
volatile uint8_t limit_x_right = 0;
volatile uint8_t limit_y_bottom = 0;
volatile uint8_t limit_y_top = 0;

volatile uint8_t toogle =0;


ZAxisController z_controller = {0};

void zaxis_one_shot_timer_init(void)
{
	// OC5B timer
	
	// WGM52 CTC Mode | (1<<CS52) + (1<<CS50) prescaler 1024
	TCCR5B = (1<<WGM52) | (1<<CS52) | (1<<CS50);
	
	// Clear COM5B1 + COM5B0 for disconnecting OC5B
	TCCR5A &= ~((1<<COM5B1) | (1<<COM5B0));

	// Enable Interrupt on Compare Value OCR5A
	TIMSK5 |= (1<<OCIE1B);
	
	OCR5A = 50061;
	
	sei();

}


void xyaxis_pwm_init(void)
{
	
	// Ausgang für X-Achse konfigurieren
	BitSet(DDRE, X_TAKT);
	BitSet(DDRA, X_DIR);
	BitSet(DDRA, X_ACTIVE);
		
	// Ausgang für Y-Achse konfigurieren
	BitSet(DDRB, Y_TAKT);
	BitSet(DDRA, Y_DIR);
	BitSet(DDRA, Y_ACTIVE);
		
	// XY-Schrittmotoren aktivieren
	XY_SET_ACTIVE;
	
	// X-Achse
	// --------------------------------------------
	// WGM32 CTC Mode | CS31 + CS30 prescaler 64
	TCCR3B = (1<<WGM32) | (1<<CS31) | (1<<CS30);
	
	// Toggle den Output Pin OC3B = PE4 = X_TAKT
	TCCR3A = (1<<COM3B0);

	// 500 Hz
	OCR3A = 249;
	// --------------------------------------------
	
	
	
	
	// Y-Achse
	// --------------------------------------------
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);

	// Toggle den Output Pin OC1B = PB6 = Y_TAKT
	TCCR1A = (1<<COM1B0);
	
	// 500 Hz
	OCR1A = 249;
	// --------------------------------------------
}


// Returns Numbers of real chars
uint8_t receive_sentence(char* buffer, uint8_t size)
{
	if (size==0)
	{
		return 0;
	}
	
	uint8_t index = 0;
	while(index  < size - 1)
	{
		char single_char = uart_rec();
		uart_send(single_char);
		
		if (single_char == 00)
		{
			break;
		}
		
		buffer[index] = single_char;
		++index;
	}
	
	buffer[index] = '\0';
	
	return index + 1;
}




// OC5B
int main(void)
{

	uart_init();	
	lcd_init();
	ADC_Laser_init();
	
// 	while(1)
// 	{
// 		uart_send_16bit(ADC_Laser_read());
// 	}
	
	
	
	
	
// 	uint8_t i = 0;						// angezeigte Variable
// 	char lcd_str[17];					// Array für Display-Ausgabe
// 	
// 	lcd_init();
// 	lcd_text("Bald Wochenende  bla");
// 	lcd_cmd(0xc0);						// Cursor positioniert
// 	
// 	
// 	
// 	lcd_text("i = ");
// 	
// 	lcd_zahl(i, lcd_str);				// Umwandlung von i in Textzeichen
// 	lcd_text(lcd_str);					// Ausgabe auf Display
	
	
	
	
	
// 	while (1)
// 	{
// 			uart_send_16bit(uart_rec());
// 	}
	
	
	
	
	char text[33] = "";
	
	while(1)
	{
		uart_send(receive_sentence(text, 33));
	}
	
	
	
	
// 	char Line[33];      // String mit maximal 32 zeichen
// 	char text[2];
// 
// 	while(1)
// 	{
// 		//uart_puts(uart_gets( Line, sizeof( Line ) ));
// 		//text = uart_rec());
// 		_delay_ms(50);
// 		lcd_text(text);
// 		_delay_ms(50);
// 		uart_send(text);
// 		_delay_ms(50);
// 	}

	
	
	
		
// 	uart_init();
// 	limit_swiches_init();
	

// 	_delay_ms(1000);
// 	XY_stop_pwm();
	

	
	
	
// 	zaxis_pwm_init_start();
// 	zaxis_one_shot_timer_init();
// 	zaxis_pwm_init_start();


// 	xyaxis_init();
// 	xyaxis_move_down(250);
	
	
// 	zaxis_init();
// 	zaxis_move_clockwise(90);
		
// 		zaxis_pwm_init_start();
		
    /* Replace with your application code */
    while (1) 
    {
		
		
// 		uart_send(limit_x_left);

		
		
	
// 		uart_send(limit_y_top);
// 		uart_send(limit_y_bottom);
// 		uart_send(limit_x_left);
// 		uart_send(limit_x_right);

// 		_delay_ms(10);
		
		
// 		while (!(UCSR3A & (1<< UDRE3)));
// 		UDR3 = j;
// 		j++;
// 		_delay_ms(100);
		
// 			UDR3 = 30;
// 			_delay_ms(100);
			
// 		if (IsBitSet(UCSR0A, UDRE0))
// 		{
// 			UDR0 = j;
// 			_delay_ms(1000);
// 		}
		
    }
}


// ***********************************************************************************************************************
// INTERRUPT SERVICE ROUTINES BEGIN
// ***********************************************************************************************************************

ISR(TIMER5_COMPB_vect)
{
	Z_stop_pwm();
	Z_disable_one_shot_timer();
	
	// 	if (toogle == 0)
	// 	{
	// 		BitSet(PORTL, Z_DIR);
	// 		toogle = 1;
	// 	}
	// 	else
	// 	{
	// 		BitClear(PORTL, Z_DIR);
	// 		toogle = 0;
	
	// 	}
	
}

ISR(TIMER5_COMPA_vect)
{
}



ISR(INT3_vect)
{
		// Invertiert denken, da Taster gedrückt wenn fallende Flanke!
		if (!IsBitSet(PIND, Y_BOTTOM_LIM_PIN))
		{
			limit_y_bottom = 1;
		}
		else
		{
			limit_y_bottom = 0;
		}
}


ISR(INT2_vect)
{
	// Invertiert denken, da Taster gedrückt wenn fallende Flanke!
	if (!IsBitSet(PIND, X_LEFT_LIM_PIN))
	{
		limit_x_left = 1;
		XY_stop_pwm();
		
	}
	else
	{
		limit_x_left = 0;
		
	}
}

ISR(INT0_vect)
{
	// Invertiert denken, da Taster gedrückt wenn fallende Flanke!
	if (!IsBitSet(PIND, X_RIGHT_LIM_PIN))
	{
		limit_x_right = 1;
	}
	else
	{
		limit_x_right = 0;
	}
}

ISR(INT1_vect)
{
	// Invertiert denken, da Taster gedrückt wenn fallende Flanke!
	if (!IsBitSet(PIND, Y_TOP_LIM_PIN))
	{
		limit_y_top = 1;
	}
	else
	{
		limit_y_top = 0;
	}
}

// ***********************************************************************************************************************
// INTERRUPT SERVICE ROUTINES END
// ***********************************************************************************************************************


