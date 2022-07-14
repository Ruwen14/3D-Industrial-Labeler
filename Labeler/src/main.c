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
#include "lib/DrawingAlphabetLib.h"
#include "lib/controll_buttons.h"

#include <avr/interrupt.h>

volatile uint8_t limit_x_left = 0;
volatile uint8_t limit_x_right = 0;
volatile uint8_t limit_y_bottom = 0;
volatile uint8_t limit_y_top = 0;

volatile uint8_t toogle =0;


ZAxisController z_controller = {0};
	
volatile StepperAxisController axis_controller = {0};

volatile StepperMotionAxisController controller = {0, 3, 3, 55, 9, 0, 0, 0, 640, STATE_IDLE, OBJECT_CYLINDER ,{}};




void GO_MEASURMENT_RANGE_AND_FIND_BALLOON_CENTER(StepperMotionAxisController* c)
{
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(c);
	

	SMAC_ADD_GO_MEASUREMENT_RANGE(c);
	for (uint8_t i = 0; i < BALLOON_SHELL_CENTER_FIND_STEPS; i++)
	{
		SMAC_ADD_FIND_BALLOON_SHELL_CENTER_STEP(c);
	}
	
	SMAC_ADD_MOVE_X_BIG(c, BALLOON_SHELL_CENTER_FIND_STEPS*1, X_LEFT);
	
	
	SMAC_END_DECLARE_MOTION_SEQUENCE(c);
}


void GO_MEASUREMENT_RADII_BALLOON(StepperMotionAxisController* c)
{
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(c);
	SMAC_ADD_MOVE_X_BIG(&controller, controller.balloon_shell_center_mm, X_RIGHT);
	for (uint8_t i = 0; i < 15; i++)
	{
		SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	}

	SMAC_ADD_MOVE_X_max82(c, 4*c->pixel_unit_mm, X_LEFT);
	for (uint8_t j = 0; j < 18; j++)
	{
		SMAC_ADD_MEAS_DIST_ROW_1(c);
	}
	SMAC_ADD_MOVE_X_BIG(c, 8*c->pixel_unit_mm, X_RIGHT);
	for (uint8_t k = 0; k < 18; k++)
	{
		SMAC_ADD_MEAS_DIST_ROW_2(c);
	}
	
}

void GO_MEASURMENT_RANGE_AND_FIND_RADIUS_CYLINDER(StepperMotionAxisController* c)
{
	
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
	SMAC_ADD_MOVE_X_max82(&controller, 80, X_RIGHT);
	SMAC_ADD_MOVE_X_max82(&controller, 80, X_RIGHT);
	SMAC_ADD_MOVE_Y_max82(&controller, 80, Y_UP);
	SMAC_ADD_MOVE_Y_max82(&controller, 80, Y_UP);
	SMAC_ADD_MOVE_Y_max82(&controller, 30, Y_UP);
	SMAC_ADD_MOVE_Y_sub_mm_max255(&controller, 7, Y_UP);
	
	SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);
	SMAC_ADD_MOVE_Y_sub_mm_max255(&controller, 5, Y_UP);
	SMAC_ADD_MOVE_Y_max82(&controller, 3, Y_DOWN);
	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
}





int main(void)
{

	// ToDo: sei() rausmachen
	uart_init();
	lcd_init();
	SMAC_init(&controller, 55, OBJECT_BALLOON);
	ADC_Laser_init();
	limit_swiches_init();
	controll_buttons_init();
	
	UserInputHandler input_handler = {0, 16, INPUT_INCOMPLETED, {}};
	uint8_t was_object_type_choosen = 0;
	uint8_t is_object_type_question_published_on_lcd = 0;
	uint8_t is_input_published_on_lcd = 0;
	uint8_t is_drawing = 0;
	uint8_t was_instructed_home = 0;
	uint8_t is_drawing_done = 0;
	uint8_t character_counter = 0;
	uint8_t was_instructed_meas_radius = 0;
	uint8_t was_instructed_find_center_point_balloon = 0;
	uint8_t was_instructed_measure_balloon_radii = 0;
	uint8_t was_pen_calibrated = 0;




	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
	SMAC_ADD_MOVE_Z_DEGREE_max233(&controller, 233, Z_CLOCKWISE);
	SMAC_ADD_MOVE_Z_DEGREE_max233(&controller, 127, Z_CLOCKWISE);
	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
	
	
// 	SMAC_return_home(&controller);
// 	SMAC_ADD_GO_MEASUREMENT_RANGE(&controller);
// 	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
// 	
// 	SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);
// 	SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);
// 	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
	

// 	uint16_t res = SMAC_calc_one_shot_timer_Z_max233_degree(233);
	
	
	
// 	GO_MEASURMENT_RANGE_AND_FIND_BALLOON_CENTER(&controller);
// 	while(controller.state == STATE_PROCESSING)
// 	{
// 		
// 	}
// 	GO_MEASUREMENT_RADII_BALLOON(&controller);
// 	while(controller.state == STATE_PROCESSING)
// 	{
// 		
// 	}
	
	
	
// 	
// 	SMAC_ADD_MOVE_X_max82(&controller, OFFSET_PEN_LASER, X_RIGHT);
// 	
// 	if (controller.dist_row_1 > controller.dist_row_2)
// 		SMAC_ADD_MOVE_Y_FLOAT_MM(&controller, controller.dist_row_1, Y_UP);
// 	else
// 		SMAC_ADD_MOVE_Y_FLOAT_MM(&controller, controller.dist_row_2, Y_UP);
// 		
// 	SMAC_ADD_MOVE_Y_max82(&controller, 10*controller.pixel_unit_mm, Y_DOWN);
// 	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);

	

	
	
	
	
	
// 	

// 	SMAC_return_home(&controller);
		
	lcd_text("AWAIT INPUT...");		
	while(1)
	{
	
		/*
		if (!was_instructed_home)
		{
			SMAC_return_home(&controller);
			was_instructed_home = 1;
		}

		if (!was_object_type_choosen)
		{
			if (!is_object_type_question_published_on_lcd)
			{
				lcd_reset();
				lcd_text("^CYLI | BALLOON^CHOOSE OBJECT...");
				is_object_type_question_published_on_lcd = 1;
			}
			
			if (IS_VERIFY_BUTTON_PRESSED)
			{
				controller.objtype = OBJECT_BALLOON;
				controller.pixel_unit_mm_y = 8;
				was_object_type_choosen = 1;
				lcd_reset();
				lcd_text("AWAIT INPUT...");		
			}
			
			else if (IS_CANCEL_BUTTON_PRESSED)
			{
				controller.objtype = OBJECT_CYLINDER;
				was_object_type_choosen = 1;
				lcd_reset();
				lcd_text("AWAIT INPUT...");
			}
			continue;
		}
		
		

		if (is_drawing_done)
		{
			continue;
		}
		
		
		
		
		if ((!is_drawing) && was_instructed_home)
		{
			// Pollt regelmäßig die Nutzereingabe (HTerm), bis der Buffer (32 Zeichen) voll ist.
			UserInputHandler_poll_input(&input_handler);
			if (input_handler.status == INPUT_COMPLETED)
			{
				// Überträgt die Nutzereingabe auf das LCD. Macht dies nur einmalig pro Nutzereingabe, um
				// redundante Aufrufe zu eliminieren.
				if (!is_input_published_on_lcd)
				{
					lcd_reset();
					lcd_text(input_handler.input_buffer);
					is_input_published_on_lcd = 1;
				}
			
				// Überprüft die Eingabe (HTerm) auf unterstützte Zeichen. Sofern invalide Zeichen -> wird Fehlermeldung ausgegeben und der Nutzer
				// muss eine neue Eingabe tätigen. Ist die Eingabe valide ist, wird in den Draw-Modus geswitched (is_ready_to_draw = 1) und das eigentliche 
				// Schreiben beginnt.
				if (IS_VERIFY_BUTTON_PRESSED)
				{
					uint8_t valid = validate_input_stream(input_handler.input_buffer);
				
					if (valid)
					{
						// Ready to Start Drawing
						is_drawing = 1;
					}
			
				}
					
				// Resettet die Nutzereingabe. Der Nutzer kann nun erneut eine Eingabe (HTerm) tätigen -> Solange bis Nutzereingabe valide!
				else if (IS_CANCEL_BUTTON_PRESSED)
				{
					UserInputHandler_reset_input(&input_handler);
					is_input_published_on_lcd = 0;
					lcd_reset();
					lcd_text("AWAIT INPUT...");
				}
			}
		
		}
		
		
		else if(is_drawing)
		{
			// Controller kann weitere Buchstaben koordinieren
			if (!(controller.state == STATE_PROCESSING))
			{
				
				if (!was_instructed_meas_radius)
				{
					if (controller.objtype == OBJECT_CYLINDER)
					{
						GO_MEASURMENT_RANGE_AND_FIND_RADIUS_CYLINDER(&controller);
						was_instructed_meas_radius = 1;
					}
					
					else if (controller.objtype == OBJECT_BALLOON)
					{
						if (!was_instructed_find_center_point_balloon)
						{
							GO_MEASURMENT_RANGE_AND_FIND_BALLOON_CENTER(&controller);
							was_instructed_find_center_point_balloon = 1;
						
						}
						
						else if (!(was_instructed_measure_balloon_radii) && was_instructed_find_center_point_balloon)
						{	
							
							GO_MEASUREMENT_RADII_BALLOON(&controller);
							SMAC_ADD_MOVE_X_max82(&controller, OFFSET_PEN_LASER, X_RIGHT);
							SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
							was_instructed_measure_balloon_radii = 1;
						}
							
						else if (was_instructed_measure_balloon_radii)
						{
							controller.temp_ocra_diagonal = calc_temp_freq_Z_for_diagonal_move(controller.steps_per_mm_z);
							
							
							uart_send_16bit(controller.temp_ocra_diagonal);
							
							if (controller.dist_row_1 > controller.dist_row_2) 
								SMAC_ADD_MOVE_Y_FLOAT_MM(&controller, controller.dist_row_1-15, Y_UP);
							else 
								SMAC_ADD_MOVE_Y_FLOAT_MM(&controller, controller.dist_row_2-15, Y_UP);
								
							SMAC_ADD_MOVE_Y_max82(&controller, controller.pixel_unit_mm_y, Y_DOWN);
							SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
							was_instructed_meas_radius = 1;
						}
					}
				}
				
				
				else
				{
					char character = input_handler.input_buffer[character_counter];

					if (!(character == '\0'))
					{
						SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
						SMAC_dispatch_character_function(&controller, character);
// 					
// 						
						if (controller.objtype == OBJECT_CYLINDER)
						{

							// Wenn Zweite Reihe (entweder forciert durch # oder natürlich nach 16 Zeichen)
							if (character_counter == input_handler.second_row_at - 1)
							{
								SMAC_go_beginning_row2(&controller, character_counter+1);
						
								character_counter = 15;

							}
						}
					
						SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
// 						uart_send(character);
			
					}
					
					else
					{
						is_drawing_done=1;
					}
					++character_counter;
				
				}
				
			}
		}
		*/
		
		
		
		
		
		
// 		uart_send_16*/
// 		uart_send_16bit(SMAC_calc_one_shot_timer_Z_max233(233, controller.steps_per_mm_z));



				
		
		
// 		uart_send_16bit(calc_steps_per_mm_Z(55));		
// 		uart_send(!IsBitSet(PIND, X_RIGHT_LIM_PIN));


// 		uint8_t e = calc_pixel_unit_width_mm(103);
// 		
		

		
// 		uart_send(controller.pixel_unit_mm);
		
		
// 		uart_send(!IsBitSet(PIND, Y_BOTTOM_LIM_PIN));
	
// 		_delay_ms(10);
		
// 		uint16_t laser_data = ADC_Laser_read();
// 		uint16_t data = laser_quantize_10th_mm(laser_data, 2, 1003, 0, 503); // In wirklichkeit 55
		
		
// 		uint16_t laser_data = ADC_Laser_read_median();
// 		uart_send_16bit(laser_data);
// // 
// // 		
// // 		
// 		uint8_t steps_per_mmz = calc_steps_per_mm_Z(rad);
// // 
// 		uart_send(steps_per_mmz);
// // 		



		
		
// 		uart_send(rad);
		
		
// 		uart_send(rad);
		
		
		
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
	{
		controller.stateflags = 0;
		controller.state = STATE_IDLE;
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
		
		TCNT3 = 0; // X-Reset Counter
		TCNT1 = 0; // Y-Reset Counter
		BitClear(PORTA, Y_DIR);
		BitClear(PORTA, X_DIR);
		controller.state = STATE_PROCESSING;
		
		
		
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
		controller.state = STATE_IDLE;
		
		
		
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
		controller.state = STATE_PROCESSING;

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
		controller.state = STATE_PROCESSING;
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




