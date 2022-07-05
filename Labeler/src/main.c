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

volatile StepperMotionAxisController controller = {0, MODE_NORMAL, {}};




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
// 	SMAC_ADD_MOVE_Z_max233(&controller, 3, Z_ANTICLOCKWISE);
// 	SMAC_ADD_MOVE_Y_max82(&controller, 2, Y_UP);
	
	
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
// 	SMAC_ADD_MOVE_Y_max82(&controller, 2, Y_DOWN);
	

	// FAhrt zur refernzlinie
// 	SMAC_ADD_MOVE_Z_max233(&controller, 5, Z_ANTICLOCKWISE);
// 	SMAC_ADD_MOVE_X_max82(&controller, 14, X_RIGHT);
	
}


uint16_t round_integers(uint16_t zahl, uint8_t n)
{
	

	
	// Hunderter abziehen
	uint8_t einer = 0;
	
	uint16_t zehner = zahl/n;					// Zehnerstelle ermitteln
	einer = zahl - zehner * n;					// Zehner abziehen
	
	if (zehner > 0 )
	{
		if (einer >= 5)
		{
			return (zehner+1) * n;
		}
		else
		{
			return zahl;
		}
	}
	
	else
	{
		return zahl;
	}
	return 0;
}


uint16_t int_div_16(uint16_t data)
{
	return ((data +8) >> 4);
}

uint16_t laser_read_average_max64(uint8_t n)
{
	uint16_t sum = 0;
	for(uint8_t i = 0; i < n; i++)
	{
		uint16_t laser_data = ADC_Laser_read();
		sum = sum + laser_quantize_10th_mm(laser_data, 2, 1003, 0, 503);
	}
	
	
// 	uart_send_16bit(sum/n);
	
// 	uart_send_16bit(round_integers(sum, n) / n);
	
	
// 	uint16_t avg = int_div_16(sum);
	
	uint16_t avg = round_integers(sum, n)  / n;
	return avg;
}




void SMAC_GO_MEAS_RANGE(StepperMotionAxisController* c, uint8_t* state)
{
	if (!c->stateflags == 0)
	{
		return;
	}
	
	switch(*state)
	{
		case 0:
			SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(c);
// 			SMAC_ADD_MOVE_X_max82(c, 60, X_RIGHT);
// 			SMAC_ADD_MOVE_X_max82(c, 60, X_RIGHT);
			SMAC_ADD_MOVE_Y_max82(c, 30, Y_UP);
			
			SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
			*state += 1;
			break;
	
		case 1:
			{
				uint16_t data = laser_read_average_max64(10);
				if (data > 450)
				{
					SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(c);
					SMAC_ADD_MOVE_Y_max82(c, 25, Y_UP);
					SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
				}
				else
				{
					*state +=1;
				}
				break;
			}
		
		case 2:
			break;
		
	}
}






				
	
	
	




void driveToDrawingLevel(void)
{
	uint16_t y_dist  = ADC_Laser_read_median();
	
	uint8_t mm = y_dist / 10;
	uint8_t tenth_mm = y_dist - mm * 10;
	
	
	if (mm != 0)
	{
		SMAC_ADD_MOVE_Y_max82(&controller, mm, Y_UP);
	}
	
	if (tenth_mm != 0)
	{
		SMAC_ADD_MOVE_Y_sub_mm_max255(&controller, tenth_mm, Y_UP);
	}
	
	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
}




void SMAC_return_home(StepperMotionAxisController* c)
{
	// Stop Current Driving
	SMAC_disable_XY_pwm();
	SMAC_disable_Z_pwm();
	SMAC_disable_multi_one_shot_timer();
	
	// Empty Buffer
	FIFOSeqBuffer_delete(&controller.sequencebuffer);
	
	
	while(!IsBitSet(PIND, X_LEFT_LIM_PIN))
	{
		BitSet(PORTA, Y_DIR);
		BitSet(PORTA, X_DIR);
		XY_RISING_EDGE;
		_delay_us(1000);
		XY_FALLING_EDGE;
		_delay_us(1000);
		
	}
	_delay_ms(1000);
	
	
	while(!IsBitSet(PIND, Y_BOTTOM_LIM_PIN))
	{
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			XY_RISING_EDGE;
			_delay_us(1000);
			XY_FALLING_EDGE;
			_delay_us(1000);
	}
	
	
	
	
	
	_delay_ms(1000);
	
	BitClear(PORTA, Y_DIR);
	BitSet(PORTA, X_DIR);
	SMAC_run_XY_pwm();
	
}






// OC5B
int main(void)
{
	
	uart_init();
	lcd_init();
	SMAC_init_XYZ();
	ADC_Laser_init();
	limit_swiches_init();
	
	
	
	
// 	BitClear(PORTA, Y_DIR);
// 	BitSet(PORTA, X_DIR);
	
// 	
// 	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
// 	SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);

// 	drawB();
// 	SMAC_ADD_MOVE_Z_max233(&controller, 200, Z_ANTICLOCKWISE);
// 	SMAC_ADD_MOVE_Z_max233(&controller, 200, Z_ANTICLOCKWISE);
	
// 	SMAC_ADD_MOVE_Z_max233(&controller, 200, Z_ANTICLOCKWISE);
	
// 	SMAC_ADD_MOVE_X_max82(&controller, 80, X_RIGHT);
// 	SMAC_ADD_MOVE_Y_max82(&controller, 80, Y_UP);
// 	SMAC_ADD_MOVE_Y_max82(&controller, 80, Y_UP);
	
// 	SMAC_ADD_MOVE_Y_max82(&controller, 80, Y_DOWN);
	
// 	SMAC_ADD_MOVE_Y_max82(&controller, 80, Y_DOWN);
// 	SMAC_ADD_MOVE_Y_max82(&controller, 80, Y_DOWN);
// 	
// 	SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);
// 	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
// 	SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);
// 	SMAC_ADD_MOVE_Y_max82(&controller, 70, Y_DOWN);
// 	SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);
// 	SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);
	
// 	
// 	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
	
// 	BitSet(PORTA, Y_DIR);
// 	BitClear(PORTA, X_DIR);
// 	SMAC_run_XY_pwm();

		SMAC_return_home(&controller);

	
	
	while(1)
	{
	
		
// 		uart_send(!IsBitSet(PIND, X_RIGHT_LIM_PIN));


		
// 		uart_send(!IsBitSet(PIND, Y_BOTTOM_LIM_PIN));
	
// 		_delay_ms(10);
		
// 		uint16_t laser_data = ADC_Laser_read();
// 		uint16_t data = laser_quantize_10th_mm(laser_data, 2, 1003, 0, 503); // In wirklichkeit 55
		
// 		uint16_t laser_data = ADC_Laser_read_median();
// 		uart_send_16bit(laser_data);
// 		
		
		
// 		uart_send(limit_x_left);
		
	}
}

// INTERRUPT TESTING

ISR(TIMER5_COMPA_vect)
{
	// Für eine ISR ist dies zugegebenermassen vergleichsweise viel Quellcode. Es sei aber bedacht, dass diese
	// ISR den Übergang zweier Fahrtsequenzen koordiniert. Dies ist weder zeitkritisch und wird auch nicht häufig aufgerufen.
	
	// TODO: Erlaubte Nested Interrupt durch Endlagenschalter.
	
	
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
	else
		controller.stateflags = 0;



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
		
		TCNT3 = 0; // X-Reset Counter
		TCNT1 = 0; // Y-Reset Counter
		BitClear(PORTA, Y_DIR);
		BitClear(PORTA, X_DIR);
		
		
		
// 		SMAC_disable_XY_pwm();
// 		SMAC_disable_Z_pwm();
// 		SMAC_disable_multi_one_shot_timer();
// 		FIFOSeqBuffer_delete(&controller.sequencebuffer);
// 		BitClear(PORTA, Y_DIR);
// 		BitClear(PORTA, X_DIR);
// 		SMAC_run_XY_pwm();
// 		uart_send(3);
			
			
// 		SMAC_disable_XY_pwm();
// 		SMAC_disable_Z_pwm();
// 		SMAC_disable_multi_one_shot_timer();
// 		
// 		BitClear(PORTA, Y_DIR);
// 		SMAC_run_XY_pwm();
		
		
	}
	// Leider brauchen wir diese Funktion weil die Endlagenschalter prallen und den Interrupt 4x statt 2x triggern;
	prevent_bounce();
}


ISR(INT2_vect)
{
	
	// Invertiert denken, da Taster gedrückt wenn fallende Flanke!
	if (!IsBitSet(PIND, X_LEFT_LIM_PIN))
	{
		
		SMAC_disable_XY_pwm();
		SMAC_disable_Z_pwm();
		SMAC_disable_multi_one_shot_timer();
		FIFOSeqBuffer_delete(&controller.sequencebuffer);
		
// 		SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
// 		SMAC_ADD_MOVE_Y_max82(&controller, 10, Y_UP);
// 		SMAC_ADD_MOVE_X_max82(&controller, 10, X_RIGHT);
// 		SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
		
	}
	// Leider brauchen wir diese Funktion weil die Endlagenschalter prallen und den Interrupt 4x statt 2x triggern;
	prevent_bounce();
	
	
}

ISR(INT0_vect)
{
	
	
	// Invertiert denken, da Taster gedrückt wenn fallende Flanke!
	if (!IsBitSet(PIND, X_RIGHT_LIM_PIN))
	{
		
		SMAC_disable_XY_pwm();
		SMAC_disable_Z_pwm();
		SMAC_disable_multi_one_shot_timer();
		FIFOSeqBuffer_delete(&controller.sequencebuffer);
		
// 		SMAC_disable_XY_pwm();
// 		SMAC_disable_Z_pwm();
// 		SMAC_disable_multi_one_shot_timer();
// 		FIFOSeqBuffer_delete(&controller.sequencebuffer);
// 		SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
// 		SMAC_ADD_MOVE_X_max82(&controller, 5, X_LEFT);
// 		SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
		
		
	}
	// Leider brauchen wir diese Funktion weil die Endlagenschalter prallen und den Interrupt 4x statt 2x triggern;
	prevent_bounce();
	
	
// 	else
// 	{
// 		limit_x_right = 0;
// 	}
}

ISR(INT1_vect)
{
	
	// Invertiert denken, da Taster gedrückt wenn fallende Flanke!
	if (!IsBitSet(PIND, Y_TOP_LIM_PIN))
	{
		SMAC_disable_XY_pwm();
		SMAC_disable_Z_pwm();
		SMAC_disable_multi_one_shot_timer();
		FIFOSeqBuffer_delete(&controller.sequencebuffer);
		//
// 		
// 		limit_y_top = 1;
// 		uart_send(1);
	}
	
	
	// Leider brauchen wir diese Funktion weil die Endlagenschalter prallen und den Interrupt 4x statt 2x triggern;
	prevent_bounce();
}
// ***********************************************************************************************************************
// INTERRUPT SERVICE ROUTINES END
// ***********************************************************************************************************************


