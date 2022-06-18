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
#include "lib/StepperAxisController.h"
#include "lib/StepperMotionAxisController.h"
#include <avr/interrupt.h>

volatile uint8_t limit_x_left = 0;
volatile uint8_t limit_x_right = 0;
volatile uint8_t limit_y_bottom = 0;
volatile uint8_t limit_y_top = 0;

volatile uint8_t toogle =0;


ZAxisController z_controller = {0};
	
volatile StepperAxisController axis_controller = {0};

volatile StepperMotionAxisController controller = {0, {}};

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

void drawRectangleWithCross(void)
{
	Z_move_max233(&axis_controller, 10, ROTATE_ANTICLOCKWISE);
	_delay_ms(2000);
	X_move_max82(&axis_controller, 10, MOVE_RIGHT);
	_delay_ms(2000);
	Z_move_max233(&axis_controller, 10, ROTATE_CLOCKWISE);
	_delay_ms(2000);
	X_move_max82(&axis_controller, 10, MOVE_LEFT);
	_delay_ms(2000);
	ZX_move_diagonal_45d(&axis_controller, 10, ROTATE_ANTICLOCKWISE, MOVE_RIGHT);
	_delay_ms(2000);
	ZX_move_diagonal_45d(&axis_controller, 5, ROTATE_ANTICLOCKWISE, MOVE_LEFT);
	_delay_ms(2000);
	ZX_move_diagonal_45d(&axis_controller, 5, ROTATE_CLOCKWISE, MOVE_LEFT);
	_delay_ms(2000);
	ZX_move_diagonal_45d(&axis_controller, 10, ROTATE_CLOCKWISE, MOVE_RIGHT);
	_delay_ms(2000);
	OCR4A = 249;
	_delay_ms(2000);
	
}

void moveRight(void)
{
	X_move_max82(&axis_controller, 80, MOVE_RIGHT);
	_delay_ms(5000);
	X_move_max82(&axis_controller, 80, MOVE_RIGHT);
	_delay_ms(5000);
	
}


void moveLeft(void)
{
	X_move_max82(&axis_controller, 80, MOVE_LEFT);
	_delay_ms(5000);
	X_move_max82(&axis_controller, 80, MOVE_LEFT);
	_delay_ms(5000);
	
}




void drawNikolausFIFO(void)
{
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
	SMAC_ADD_MOVE_Z_max233(&controller, 20, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_X_max82(&controller, 20, X_RIGHT);
	SMAC_ADD_MOVE_Z_max233(&controller, 20, Z_CLOCKWISE);
	SMAC_ADD_MOVE_X_max82(&controller, 20, X_LEFT);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 20, Z_ANTICLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 10, Z_ANTICLOCKWISE, X_LEFT);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 10, Z_CLOCKWISE, X_LEFT);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 20, Z_CLOCKWISE, X_RIGHT);
	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);

}


void drawM(void)
{
	// Fahrt zur Start Linie
	SMAC_ADD_MOVE_Z_max233(&controller, 3, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_Y_max82(&controller, 2, Y_UP);
	
		
	SMAC_ADD_MOVE_X_max82(&controller, 28, X_LEFT);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 7, Z_ANTICLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 7, Z_ANTICLOCKWISE, X_LEFT);
	SMAC_ADD_MOVE_X_max82(&controller, 28, X_RIGHT);
	SMAC_ADD_MOVE_Y_max82(&controller, 2, Y_DOWN);
	
	
	// Fahrt zur Referenzlinie
	
	SMAC_ADD_MOVE_Z_max233(&controller, 3, Z_ANTICLOCKWISE);
	
}

void drawB(void)
{
	// Fahrt zur Start Linie
	SMAC_ADD_MOVE_Z_max233(&controller, 3, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_Y_max82(&controller, 2, Y_UP);
	
	
	SMAC_ADD_MOVE_X_max82(&controller, 28, X_LEFT);
	SMAC_ADD_MOVE_Z_max233(&controller, 12, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 2, Z_ANTICLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_X_max82(&controller, 10, X_RIGHT);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 2, Z_CLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 2, Z_ANTICLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_X_max82(&controller, 10, X_RIGHT);
	SMAC_ADD_MOVE_45DIAGONAL(&controller, 2, Z_CLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_Z_max233(&controller, 12, Z_CLOCKWISE);
	SMAC_ADD_MOVE_X_max82(&controller, 14, X_LEFT);
	SMAC_ADD_MOVE_Z_max233(&controller, 12, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_Y_max82(&controller, 2, Y_DOWN);
	

	// FAhrt zur refernzlinie
	SMAC_ADD_MOVE_Z_max233(&controller, 5, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_X_max82(&controller, 14, X_RIGHT);
	
}




// OC5B
int main(void)
{
	uart_init();
	lcd_init();
	XYZ_init();
	
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
	
	SMAC_ADD_MOVE_Y_max82(&controller, 2, Y_DOWN);
	
	for (uint8_t i = 0; i < 4; i++)
	{
		drawM();
		drawB();
		drawM();
		drawB();
	}
	
	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
// 	
// // // 	
// 	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
// 	SMAC_ADD_MOVE_Y_sub_mm_max255(&controller, 15, Y_UP);
// 	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
	
	
	
	
	
// 	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
	
// 	ADC_Laser_init();
// 	
// 	SMAC_ADD_MOVE_Y_max82(&controller, 80, Y_DOWN);
// 	
// 	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
	
	
	while(1)
	{
		
	}
	
	
	
}

// INTERRUPT TESTING

ISR(TIMER5_COMPA_vect)
{
	SMAC_disable_XY_pwm();
	SMAC_disable_Z_pwm();
	SMAC_disable_multi_one_shot_timer();
	OCR4A = 249; // Reset sofern wir diagonal gefahren sind.
	if (!(FIFOSeqBuffer_empty(&controller.sequencebuffer)))
	{
		MotionSequence seq;
		FIFOSeqBuffer_pop(&controller.sequencebuffer, &seq);
		SMAC_start_new_motion_sequence(&controller, &seq);
	}
	
	
}

// ***********************************************************************************************************************
// INTERRUPT SERVICE ROUTINES BEGIN
// ***********************************************************************************************************************

// Verantwortlich für das Ausschalten der Stepper-PWM nach dem Prinzip des One-Shot-Timer's.
// Sofern Zwei Achsen gleichzeitig betrieben werden (XZ oder YZ), werden diese über TIMER5_COMPB_vect + TIMER5_COMPA_vect
// synchronisiert (2x One-Shot-Timer aus)
// ISR(TIMER5_COMPA_vect)
// {
// 	// Dieser ISR wird nach TIMER5_COMPB_vect aufgerufen
// 	// Egal ob wir ein- oder zweiachsig verfahren, können wir hier einfach
// 	// alle PWM's ausschalten. 
// 	XY_disable_pwm();
// 	Z_disable_pwm();
// 	
// 	// Im Falle beim einachsigen Verfahren, schaltet 'disable_multi_one_shot_timer'
// 	// auch automatisch den 'Single'-One-Shot-Timer aus.
// 	disable_multi_one_shot_timer();
// 	RESET_ALL_AXIS_FLAGS(axis_controller.stateflags);
// 	uart_string("TimerA");
// 	
// 	
// }


// Sofern Zwei Achsen gleichzeitig betrieben werden (XZ oder YZ) kommt der 'TIMER5_COMPB_vect' zusätzlich zum Einsatz.
// Dieser stoppt immer diejenige der beiden Achsen, welche kürzer betrieben wird, um den geforderten Weg zu verfahren.
// Aus der Bedingung OCR5B < OCR5A (siehe S. 146 Atmega2560 Datenblatt), wird diese ISR immer vor der obigen ISR aufgerufen.
// ISR(TIMER5_COMPB_vect)
// {
// 	if (IS_DUAL_AXIS_MOVING(axis_controller.stateflags))
// 	{
// 		if (IS_Z_USING_OCR5B(axis_controller.stateflags))
// 		{
// 			uart_string("TimerB_disableZ");
// 	
// 			Z_disable_pwm();
// 		}
// 		else
// 		{
// 			uart_string("TimerB_disableXY");
// 			XY_disable_pwm();
// 		}
// 		DISABLE_TIMER5_COMPB_vect;
// 	}
// }



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


