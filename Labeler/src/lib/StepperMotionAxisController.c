/*
 * StepperMotionAxisController.c
 *
 * Created: 13.06.2022 21:20:12
 *  Author: ruwen
 */ 

#include "StepperMotionAxisController.h"
#include "uart.h"



uint8_t FIFOSeqBuffer_empty(FIFOSeqBuffer* buf)
{
	if (buf->readIndex == buf->writeIndex)
		return 1;
	return 0;
}

FIFOSeqBufferOptSuccess FIFOSeqBuffer_push(FIFOSeqBuffer* buf, MotionSequence seq)
{
	uint8_t next = ((buf->writeIndex + 1) & FIFO_BUFFER_MASK);

	if (buf->readIndex == next)
		return BUFFER_FULL; // voll

	buf->sequences[buf->writeIndex] = seq;
	// 	// buf->data[buf->write & BUFFER_MASK] = byte; // absolut Sicher
	buf->writeIndex = next;

	return BUFFER_HAS_CAPACITY;
}

FIFOSeqBufferOptSuccess FIFOSeqBuffer_pop(FIFOSeqBuffer* buf, MotionSequence* seq)
{
	if (buf->readIndex == buf->writeIndex)
		return BUFFER_EMPTY;

	*seq = buf->sequences[buf->readIndex];

	buf->readIndex = (buf->readIndex + 1) & FIFO_BUFFER_MASK;

	return BUFFER_HAS_CAPACITY;
}

void FIFOSeqBuffer_delete(FIFOSeqBuffer* buf)
{
	MotionSequence seq;
	
	while(FIFOSeqBuffer_pop(buf, &seq) == BUFFER_HAS_CAPACITY)
	{
		
	}
	
	
}


void SMAC_init_XYZ(void)
{
	// ZXY-STEPPER-PORTS
	// Ausgang für Z-Achse konfigurieren
	BitSet(DDRH, Z_TAKT);
	BitSet(DDRL, Z_DIR);
	BitSet(DDRL, Z_ACTIVE);
	
	// Z-Schrittmotor aktivieren
	Z_SET_ACTIVE;
	
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
	
	// Z-Stepper 500 Hz
	OCR4A = 249;
	
	
	// X-Stepper 500 Hz
	OCR3A = 249;
	
	// Y-Stepper 500 Hz
	OCR1A = 249;
}

void SMAC_set_dir_ports(AxisCmdFlag cmd)
{
	 // Wird wahrscheinlich zu einem Jump-Table optimiert.
	 // Performance entsprechend O(1) -> const.
	switch (cmd)
	{
		case AXIS_MOVE_LEFT:
			BitClear(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			break;
			
		case AXIS_MOVE_RIGHT:
			BitSet(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			break;

		case AXIS_MOVE_UP:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			break;

		case AXIS_MOVE_DOWN:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			break;

		case AXIS_ROTATE_CLOCKWISE:
			BitClear(PORTL, Z_DIR);
			break;

		case AXIS_ROTATE_ANTICLOCKWISE:
			BitSet(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_RIGHT_ROTATE_CLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_LEFT_ROTATE_CLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_RIGHT_ROTATE_ANTICLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_LEFT_ROTATE_ANTICLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_UP_ROTATE_CLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_DOWN_ROTATE_CLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_UP_ROTATE_ANTICLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_DOWN_ROTATE_ANTICLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			break;

		default:
		break;
	}
}

void SMAC_run_Z_pwm(void)
{
	// WGM42 CTC Mode | CS41 + CS40 -> prescaler 64
	TCCR4B = (1<<WGM42) | (1<<CS41) | (1<<CS40);

	// Reset Counter
	TCNT4 = 0;

	// Toggle den Output Pin OC4B = PH4 = Z_TAKT
	TCCR4A |= (1<<COM4B0);
}

void SMAC_disable_Z_pwm(void)
{
	// No Source (Timer/Counter stopped)
	TCCR4B = 0;

	// Wahrscheinlich nicht benötigt ??? ToDo: Ruwen
	TCCR4A = 0;
}

void SMAC_run_XY_pwm(void)
{
	// X-Achse
	// --------------------------------------------
	// WGM32 CTC Mode | CS31 + CS30 prescaler 64
	TCCR3B = (1<<WGM32) | (1<<CS31) | (1<<CS30);
	
	
	// Y-Achse
	// --------------------------------------------
	// WGM12 CTC Mode | CS11 + CS10 prescaler 64
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);
	
	
	TCNT3 = 0; // X-Reset Counter
	TCNT1 = 0; // Y-Reset Counter
	
	
	
	// X-Achse
	// --------------------------------------------
	// Toggle den Output Pin OC3B = PE4 = X_TAKT
	TCCR3A = (1<<COM3B0);
	
	
	// Y-Achse
	// --------------------------------------------
	// Toggle den Output Pin OC1B = PB6 = Y_TAKT
	TCCR1A = (1<<COM1B0);
}

void SMAC_disable_XY_pwm(void)
{
	TCCR3B = 0;
	TCCR1B = 0;
	
	TCCR3A = 0;
	TCCR1A = 0;
}

uint16_t SMAC_calc_one_shot_timer_Z_max233(uint8_t move_mm, uint8_t steps_per_mm_z)
{
	uint8_t pulse_period_ms = 2;

	// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
	uint32_t T_interrupt_period = move_mm * steps_per_mm_z * pulse_period_ms;

	uint32_t OCRNA_ = (T_interrupt_period * 15625) - 1000; // (16 MHz / N=1024) = 15625

	uint16_t OCRNA_rescaled_SI = OCRNA_ / 1000;

	return OCRNA_rescaled_SI;
}

uint16_t SMAC_calc_one_shot_timer_XY_max82(uint8_t move_mm)
{
	// Folgende Operationen sind bestimmt auch als Shifting-Operationen durchzuführen.
	// Da diese Funktion jedoch vor der PWM-Generationen/One-Shot-Start aufgerufen wird
	// und somit nicht zeitkritisch ist, habe ich mir mal die Mühe ersparrt :). Ruwen

	uint8_t pulse_period_ms = 2;

	// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
	uint32_t T_interrupt_period = move_mm * STEPS_PER_MM_X * pulse_period_ms;

	uint32_t OCRNA_ = (T_interrupt_period * 15625) - 1000;

	uint16_t OCRNA_rescaled_SI = OCRNA_ / 1000;

	return OCRNA_rescaled_SI;
}


uint16_t SMAC_calc_one_shot_timer_XY_10th_mm_max820(uint16_t move_10thmm)
{
// Folgende Operationen sind bestimmt auch als Shifting-Operationen durchzuführen.
// Da diese Funktion jedoch vor der PWM-Generationen/One-Shot-Start aufgerufen wird
// und somit nicht zeitkritisch ist, habe ich mir mal die Mühe ersparrt :). Ruwen

	uint8_t pulse_period_ms = 2;

	// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
	uint64_t T_interrupt_period = (move_10thmm * STEPS_PER_MM_X * pulse_period_ms);

	uint64_t OCRNA_ = (T_interrupt_period * 15625) - 1000;

	uint64_t OCRNA_rescaled_SI = (OCRNA_ / 1000);

	return OCRNA_rescaled_SI/10;	
}




uint16_t SMAC_calc_one_shot_timer_Z_max233_degree(uint8_t degree)
{
// 		uint8_t pulse_period_ms = 2;
// 
// 		// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
// 		uint32_t T_interrupt_period = degree * STEPS_PER_DEGREE_Z * pulse_period_ms;
// 
// 		uint32_t OCRNA_ = (T_interrupt_period * 15625) - 1000;
// 
// 		uint16_t OCRNA_rescaled_SI = OCRNA_ / 1000;
// 
// 		return OCRNA_rescaled_SI;
		
		
		uint8_t pulse_period_ms = 2;

		uint32_t temp = degree * pulse_period_ms;
		// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
		uint64_t T_interrupt_period = temp* 889;

		uint64_t OCRNA_ = (T_interrupt_period * 15625) - 1000;
		uint64_t divider = 100000;

		uint16_t OCRNA_rescaled_SI = OCRNA_ / divider;

		return OCRNA_rescaled_SI;
		

	
}

void SMAC_start_one_shot_timer(uint16_t ocrna_)
{
	// OC5A Timer-Register CTC
	OCR5A = ocrna_;
	
	// Reset da in *disable* potenziell cleared.
	// WGM52 CTC Mode | (1<<CS52) + (1<<CS50) prescaler 1024
	TCCR5B = (1<<WGM52) | (1<<CS52) | (1<<CS50);
	
	// Clear COM5A1 + COM5A0 for disconnecting OC5A. Usable for Normal port operation (p. 155)
	TCCR5A &= ~((1<<COM5A1) | (1<<COM5A0));
	
	// Enables Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 |= (1<<OCIE5A);
	
	sei();
}

void SMAC_disable_one_shot_timer(void)
{
	// DISABLES Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 &= ~(1 << OCIE5A);
	
	// No clock source. (Timer / Counter stopped)
	TCCR5B &= ~((1 << CS51) | (1 << CS52) | (1 << CS50));
}

void SMAC_start_multi_one_shot_timer(uint16_t ocrna_, uint16_t ocrnb_)
{
	// Wir teilen das OC5A/B Timer-Register für CTC
	OCR5A = ocrna_;
	OCR5B = ocrnb_;
	
	
	// Reset da in *disable* potenziell cleared.
	// WGM52 CTC Mode | (1<<CS52) + (1<<CS50) prescaler 1024
	TCCR5B = (1<<WGM52) | (1<<CS52) | (1<<CS50);
	
	// Clear COM5B1 + COM5B0 for disconnecting OC5B. Usable for Normal port operation (p. 155)
	TCCR5A &= ~((1<<COM5B1) | (1<<COM5B0));
	// Clear COM5A1 + COM5A0 for disconnecting OC5A. Usable for Normal port operation (p. 155)
	TCCR5A &= ~((1<<COM5A1) | (1<<COM5A0));
	
	// Reset da in *disable* potenziell cleared.
	// Enables Output Compare Match Interrupt 'TIMER5_COMPB_vect'
	TIMSK5 |= (1<<OCIE5B);
	// Enables Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 |= (1<<OCIE5A);
	
	sei();
}

void SMAC_disable_multi_one_shot_timer(void)
{
	// DISABLES Output Compare Match Interrupt 'TIMER5_COMPB_vect'
	TIMSK5 &= ~(1 << OCIE5B);
	// DISABLES Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 &= ~(1 << OCIE5A);
	
	// No clock source. (Timer / Counter stopped)
	TCCR5B &= ~((1 << CS51) | (1 << CS52) | (1 << CS50));
}












void SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(StepperMotionAxisController* c)
{

}

void SMAC_END_DECLARE_MOTION_SEQUENCE(StepperMotionAxisController* c)
{
	// Trigger First sequence
	if (!(FIFOSeqBuffer_empty(&c->sequencebuffer)))
	{
// 		c->stateflags = 1;
		MotionSequence seq;
		FIFOSeqBuffer_pop(&c->sequencebuffer, &seq);
		SMAC_start_new_motion_sequence(c, &seq);
	}
}

void SMAC_ADD_MOVE_X_max82(StepperMotionAxisController* c, uint8_t move_mm, XDir dir)
{
	AxisCmdFlag newCmd;
	if (dir == X_LEFT)
		newCmd = AXIS_MOVE_LEFT;
	else
		newCmd = AXIS_MOVE_RIGHT;
		
	MotionSequence seq = {move_mm, 0, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

void SMAC_ADD_MOVE_Y_max82(StepperMotionAxisController* c, uint8_t move_mm, YDir dir)
{
	AxisCmdFlag newCmd;
	if (dir == Y_UP)
		newCmd = AXIS_MOVE_UP;
	else
		newCmd = AXIS_MOVE_DOWN;
		
	MotionSequence seq = {move_mm, 0, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

void SMAC_ADD_MOVE_Z_max233(StepperMotionAxisController* c, uint8_t move_mm, ZDir dir)
{
	AxisCmdFlag newCmd;
	if (dir == Z_CLOCKWISE)
		newCmd = AXIS_ROTATE_CLOCKWISE;
	else
		newCmd = AXIS_ROTATE_ANTICLOCKWISE;
	
	MotionSequence seq = {0, move_mm, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}	



void SMAC_ADD_MOVE_Z_DEGREE_max233(StepperMotionAxisController* c, uint8_t degree, ZDir dir)
{
	
	AxisCmdFlag newCmd;
	if (dir == Z_CLOCKWISE)
		newCmd = AXIS_ROTATE_CLOCKWISE_DEGREE;
	else
		newCmd = AXIS_ROTATE_ANTICLOCKWISE_DEGREE;
	
	MotionSequence seq = {0, degree, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

void SMAC_ADD_MOVE_45DIAGONAL(StepperMotionAxisController* c, uint8_t zx_mm, ZDir zdir, XDir xdir)
{
	AxisCmdFlag newCmd;
	if (zdir == Z_CLOCKWISE)
	{
		if (xdir == X_LEFT)
			newCmd = AXIS_MOVE_LEFT_ROTATE_CLOCKWISE;
		else
			newCmd = AXIS_MOVE_RIGHT_ROTATE_CLOCKWISE;
	}
	
	else
	{
		if (xdir == X_LEFT)
			newCmd = AXIS_MOVE_LEFT_ROTATE_ANTICLOCKWISE;
		else
			newCmd = AXIS_MOVE_RIGHT_ROTATE_ANTICLOCKWISE;
	}
	
	MotionSequence seq = {zx_mm, zx_mm, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

void SMAC_ADD_MOVE_Y_sub_mm_max255(StepperMotionAxisController* c, uint8_t y_10th_mm , YDir dir)
{
	AxisCmdFlag newCmd;
	if (dir == Y_UP)
		newCmd = AXIS_MOVE_UP_SUB_MM;
	else
		newCmd = AXIS_MOVE_DOWN_SUB_MM;

	// 5 entspricht 0.5 in diesem Fall
	MotionSequence seq = {y_10th_mm, 0, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

void SMAC_ADD_MOVE_MEAS_RANGE(StepperMotionAxisController* c)
{
// 	MotionSequence seq = {25, 0, AXIS_MOVE_MEAS_RANGE};
// 	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
	
}

void SMAC_ADD_MOVE_Y_DRAWING_LEVEL(StepperMotionAxisController* c)
{
	MotionSequence seq = {0, 0, AXIS_MOVE_DRAWING_LEVEL};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

void SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(StepperMotionAxisController* c)
{
	MotionSequence seq = {0,0, AXIS_MEAS_RADIUS_MEDIAN};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

void SMAC_ADD_MEAS_DIST_ROW_1(StepperMotionAxisController* c)
{
	MotionSequence seq = {0,0, AXIS_MEAS_DIST_ROW_1};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

void SMAC_ADD_MEAS_DIST_ROW_2(StepperMotionAxisController* c)
{
	MotionSequence seq = {0,0, AXIS_MEAS_DIST_ROW_2};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

void SMAC_ADD_FIND_BALLOON_SHELL_CENTER_STEP(StepperMotionAxisController* c)
{
	MotionSequence seq = {0,0, AXIS_MEAS_BALLOON_SHELL_CENTER};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

static void _trigger_motion_X(StepperMotionAxisController* c, MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_max82(seq->move_XY);
	
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_XY_pwm();
}

static void _trigger_motion_Y(StepperMotionAxisController* c, MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_max82(seq->move_XY);
	
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_XY_pwm();
}

static void _trigger_motion_Y_sub_mm(StepperMotionAxisController* c, MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_10th_mm_max820(seq->move_XY);
	
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_XY_pwm();
}

static void _trigger_motion_Z(StepperMotionAxisController* c, MotionSequence* seq)
{

	uint16_t oneshot_val = SMAC_calc_one_shot_timer_Z_max233(seq->move_Z, c->steps_per_mm_z);
	
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_Z_pwm();	
	
}

static void _trigger_motion_Z_degree(StepperMotionAxisController*c , MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_Z_max233_degree(seq->move_Z);
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_Z_pwm();
}



static void _trigger_motion_diagonal(StepperMotionAxisController*c, MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_max82(seq->move_XY);
	
	// Halbiere Frequenz sodass gleicher Weg in gleicher Zeit verfahren wird
	OCR4A = c->temp_ocra_diagonal;
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_XY_pwm();
	SMAC_run_Z_pwm();
}

static void _trigger_motion_drawing_level(StepperMotionAxisController*c, MotionSequence* seq)
{
// 	_delay_ms(1000);	
	uint16_t y_dist  = ADC_Laser_read_median();
	
	
	

	
	uint8_t mm = y_dist / 10;
	uint8_t tenth_mm = y_dist - mm * 10;
	
	uint16_t oneshot_val = 0;
	if (mm != 0)
	{
		oneshot_val = oneshot_val + SMAC_calc_one_shot_timer_XY_max82(mm);
	}
	
	
	if (tenth_mm != 0)
	{
		oneshot_val = oneshot_val + SMAC_calc_one_shot_timer_XY_10th_mm_max820(tenth_mm+2);
	}
	
	if (oneshot_val > 0)
	{
		SMAC_start_one_shot_timer(oneshot_val);
		SMAC_run_XY_pwm();
	}
}

static void _trigger_meas_radius_step(StepperMotionAxisController*c , MotionSequence* seq)
{
	static uint16_t laser_dist_buffer[MEDIAN_FILTER_SIZE];
	static uint8_t index = 0;
	
	uint16_t laser_dist = ADC_Laser_read_median();
	laser_dist_buffer[index] = laser_dist;
	
	index++;
	
	if (index == MEDIAN_FILTER_SIZE)
	{
		uint16_t median_dist = median_filter(laser_dist_buffer);
		c->object_radius_mm  = calc_radius(median_dist);
		//c->pixel_unit_mm = calc_pixel_unit_width_mm(c->object_radius_mm);
		c->pixel_unit_mm = 3;
		c->steps_per_mm_z = calc_steps_per_mm_Z(c->object_radius_mm);
		
		index = 0;
	}
	
	// Fahre der MEDIAN_FILTER ist groß -> 15*24° = 360°
	MotionSequence seq_temp = {0, 24, AXIS_ROTATE_CLOCKWISE_DEGREE};
	_trigger_motion_Z_degree(c, &seq_temp);
}

static void _trigger_meas_dist_row_1_step(StepperMotionAxisController* c, MotionSequence* seq)
{
	// 20° Schritte 
	static uint16_t laser_dist_buffer[18];
	static uint8_t index = 0;
	
	uint16_t laser_dist = ADC_Laser_read_median();
	laser_dist_buffer[index] = laser_dist;
	
	index++;
	
	if (index == 18)
	{
		uint16_t max = laser_dist_buffer[0];
		for (uint8_t i = 0; i < 18; i++) 
		{
			if (laser_dist_buffer[i] > max)
			{
				max = laser_dist_buffer[i];
			}
		}
		
		c->dist_row_1 = max;
		index = 0;	
	}
	
	MotionSequence seq_temp = {0, 20, AXIS_ROTATE_CLOCKWISE_DEGREE};
	_trigger_motion_Z_degree(c, &seq_temp);
}



static void _trigger_meas_dist_row_2_step(StepperMotionAxisController* c, MotionSequence* seq)
{
	// 20° Schritte
	static uint16_t laser_dist_buffer_2[18];
	static uint8_t index_2 = 0;
	
	uint16_t laser_dist = ADC_Laser_read_median();
	laser_dist_buffer_2[index_2] = laser_dist;
	
	index_2++;
	
	if (index_2 == 18)
	{
		uint16_t max_2 = laser_dist_buffer_2[0];
		for (uint8_t i = 0; i < 18; i++)
		{
			if (laser_dist_buffer_2[i] > max_2)
			{
				max_2 = laser_dist_buffer_2[i];
			}
		}
		
		c->dist_row_2 = max_2;
		index_2 = 0;
	}
	
	MotionSequence seq_temp = {0, 20, AXIS_ROTATE_CLOCKWISE_DEGREE};
	_trigger_motion_Z_degree(c, &seq_temp);
}



static void _trigger_find_balloon_shell_center_step(StepperMotionAxisController* c , MotionSequence* seq)
{
	static uint16_t laser_distances[BALLOON_SHELL_CENTER_FIND_STEPS];
	static uint8_t array_index = 0;
	static uint8_t min_step_occurance = 0;
	
	uint16_t laser_dist = ADC_Laser_read_median();
	laser_distances[array_index] = laser_dist;
	array_index++;
	
	if (array_index == BALLOON_SHELL_CENTER_FIND_STEPS)
	{
		uint16_t min = laser_distances[0];
		
		for (uint8_t i = 0; i < BALLOON_SHELL_CENTER_FIND_STEPS; i++)
		{
			if (laser_distances[i] < min)
			{
				min = laser_distances[i];
				min_step_occurance = i;
			}
			
		}
		c->balloon_shell_center_mm = min_step_occurance * 1;
		array_index = 0;
	}
	
	MotionSequence seq_temp = {1, 0, AXIS_MOVE_RIGHT};
	SMAC_start_new_motion_sequence(c, &seq_temp);
}










void SMAC_start_new_motion_sequence(StepperMotionAxisController* c, MotionSequence* seq)
{
	c->state = STATE_PROCESSING;
	// Wird wahrscheinlich zu einem Jump-Table optimiert.
	// Performance entsprechend O(1) -> const.
	// Der Übergang zweier Fahrtsequenzen ist NICHT spürbar	
	switch (seq->cmd)
	{
		case AXIS_MOVE_LEFT:
			BitClear(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			
			_trigger_motion_X(c, seq);
			break;
		
		case AXIS_MOVE_RIGHT:
			BitSet(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			
			_trigger_motion_X(c, seq);
			break;

		case AXIS_MOVE_UP:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			
			_trigger_motion_Y(c, seq);
			break;

		case AXIS_MOVE_DOWN:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			
			_trigger_motion_Y(c, seq);
			break;

		case AXIS_ROTATE_CLOCKWISE:
			BitClear(PORTL, Z_DIR);
			
			_trigger_motion_Z(c, seq);
			break;

		case AXIS_ROTATE_ANTICLOCKWISE:
			BitSet(PORTL, Z_DIR);
			
			_trigger_motion_Z(c, seq);
			break;
			
			
		case AXIS_ROTATE_CLOCKWISE_DEGREE:
			BitClear(PORTL, Z_DIR);
			
			_trigger_motion_Z_degree(c, seq);
			break;
			
		case AXIS_ROTATE_ANTICLOCKWISE_DEGREE:
			BitSet(PORTL, Z_DIR);
			
			_trigger_motion_Z_degree(c, seq);
			
			break;

		case AXIS_MOVE_RIGHT_ROTATE_CLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			
			_trigger_motion_diagonal(c, seq);
			break;

		case AXIS_MOVE_LEFT_ROTATE_CLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			
			_trigger_motion_diagonal(c, seq);
			break;

		case AXIS_MOVE_RIGHT_ROTATE_ANTICLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			
			_trigger_motion_diagonal(c, seq);
			
			break;

		case AXIS_MOVE_LEFT_ROTATE_ANTICLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			
			_trigger_motion_diagonal(c, seq);
			break;

		case AXIS_MOVE_UP_ROTATE_CLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_DOWN_ROTATE_CLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_UP_ROTATE_ANTICLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_DOWN_ROTATE_ANTICLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			break;
			
		case AXIS_MOVE_DOWN_SUB_MM:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			
			_trigger_motion_Y_sub_mm(c, seq);
			break;
			
			
		case AXIS_MOVE_UP_SUB_MM:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			
			_trigger_motion_Y_sub_mm(c, seq);
			break;
			
		case AXIS_MOVE_MEAS_RANGE:
// 			BitSet(PORTA, Y_DIR);
// 			BitClear(PORTA, X_DIR);
			
			// Wir sind im Meas_Range_Anfahrweg
// 			c->stateflags = 23;
			
// 			_trigger_motion_Y(c, seq);
			
			
			break;
			
		case AXIS_MOVE_DRAWING_LEVEL:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			
			_trigger_motion_drawing_level(c, seq);
	
			break;
			
			
		case AXIS_MEAS_RADIUS_MEDIAN:
			_trigger_meas_radius_step(c, seq);
		
			break;	

		case AXIS_MEAS_DIST_ROW_1:
			_trigger_meas_dist_row_1_step(c, seq);
			
			break;
			
		case AXIS_MEAS_DIST_ROW_2:
			_trigger_meas_dist_row_2_step(c, seq);
			break;
			
		case AXIS_MEAS_BALLOON_SHELL_CENTER:
			_trigger_find_balloon_shell_center_step(c, seq);
			break;


		default:
		break;
	}
}



void SMAC_return_home(StepperMotionAxisController* c)
{
	
	
	// Stop Current Driving
	SMAC_disable_XY_pwm();
	SMAC_disable_Z_pwm();
	SMAC_disable_multi_one_shot_timer();
	
	// Empty Buffer
	FIFOSeqBuffer_delete(&c->sequencebuffer);
	c->state = STATE_PROCESSING;
	
	// Zugegebenermaßen ist das benutzen einer While-Schleife nicht perfekt und
	// widerspricht dem Design-Ansatz des Totzeitfreien Fahrens über Buffer des restlichen Codes
	// Diese Whileschleife positioniert den Motor allerdings nur um wenige Zehntel-mm vom Endlagenschalter weg und wird
	// nur einmalig am Start des Programmes ausgeführt. Die dadurch eingeführte Totzeit von wenigen millisekunden ist für uns somit vertretbar und
	// vereinfacht die Initiale-Positioniertung des Motors ERHEBLICH!
	while(!IsBitSet(PIND, X_LEFT_LIM_PIN))
	{
		BitSet(PORTA, Y_DIR);
		BitSet(PORTA, X_DIR);
		XY_RISING_EDGE;
		_delay_us(1000);
		XY_FALLING_EDGE;
		_delay_us(1000);
		
	}
	
	
	while(!IsBitSet(PIND, Y_BOTTOM_LIM_PIN))
	{
		BitSet(PORTA, Y_DIR);
		BitClear(PORTA, X_DIR);
		XY_RISING_EDGE;
		_delay_us(1000);
		XY_FALLING_EDGE;
		_delay_us(1000);
	}
	
	
	// Fahr in Home-Position
	BitClear(PORTA, Y_DIR);
	BitSet(PORTA, X_DIR);
	SMAC_run_XY_pwm();
	
}


void SMAC_ADD_MOVE_RADIUS_MEAS_RANGE(StepperMotionAxisController* c, uint8_t x_offset)
{
	SMAC_ADD_MOVE_X_max82(c, x_offset, X_RIGHT);
	
	// Gehe 190,7mm hoch. Hierfür ist der Laser optimal kalibriert
	SMAC_ADD_MOVE_Y_max82(c, 80, Y_UP);
	SMAC_ADD_MOVE_Y_max82(c, 80, Y_UP);
	SMAC_ADD_MOVE_Y_max82(c, 30, Y_UP);
	SMAC_ADD_MOVE_Y_sub_mm_max255(c, 7, Y_UP);
}

void SMAC_ADD_GO_MEASUREMENT_RANGE(StepperMotionAxisController*c)
{
		if (c->objtype == OBJECT_BALLOON)
		{
			SMAC_ADD_MOVE_X_max82(c, 77, X_RIGHT);
		}
		else
		{
			SMAC_ADD_MOVE_X_max82(c, 80, X_RIGHT);
			SMAC_ADD_MOVE_X_max82(c, 80, X_RIGHT);
		}
	
	
		// Gehe 190,7mm hoch. Hierfür ist der Laser optimal kalibriert
		SMAC_ADD_MOVE_Y_max82(c, 80, Y_UP);
		SMAC_ADD_MOVE_Y_max82(c, 80, Y_UP);
		SMAC_ADD_MOVE_Y_max82(c, 30, Y_UP);
		SMAC_ADD_MOVE_Y_sub_mm_max255(c, 7, Y_UP);
}

void SMAC_GO_AND_MEASURE_RADIUS(StepperMotionAxisController* c)
{
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(c);
	
	// X-offset
	SMAC_ADD_MOVE_X_max82(c, 80, X_RIGHT);
	SMAC_ADD_MOVE_X_max82(c, 55, X_RIGHT);

	// 190,7 mm von HOME ausgehend
	SMAC_ADD_MOVE_Y_max82(c, 80, Y_UP);
	SMAC_ADD_MOVE_Y_max82(c, 80, Y_UP);
	SMAC_ADD_MOVE_Y_max82(c, 30, Y_UP);
	SMAC_ADD_MOVE_Y_sub_mm_max255(c, 7, Y_UP);
	
	// Alle 24° eine Abstandsmessung die gemittelt(median) wird
	// und in controller.object_radius_mm gespeichert wird
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	
	SMAC_END_DECLARE_MOTION_SEQUENCE(c);
}



void SMAC_ADD_MOVE_X_BIG(StepperMotionAxisController* c, uint8_t move_mm, XDir dir)
{
	while(move_mm != 0)
	{
		if (move_mm >= 80)
		{
			
			SMAC_ADD_MOVE_X_max82(c, 80, dir);
			move_mm -= 80;
		}
		else
		{
			SMAC_ADD_MOVE_X_max82(c, move_mm, dir);
			move_mm -= move_mm;
		}
	}
	
}



void SMAC_ADD_MOVE_Y_FLOAT_MM(StepperMotionAxisController*c, uint16_t move, YDir dir)
{
	uint8_t mm = move / 10;
	uint8_t tenth_mm = move - mm * 10;
	
	if (mm != 0)
	{
		SMAC_ADD_MOVE_Y_max82(c, mm, dir);
	}
	
	if (tenth_mm != 0)
	{
		SMAC_ADD_MOVE_Y_sub_mm_max255(c, tenth_mm, dir);
	}
}


uint8_t calc_steps_per_mm_Z(uint16_t radius_mm)
{
	
	uint16_t presc = Z_STEPS_PER_MM_PRESCALER;
	uint32_t scaled_radius = 10000 / radius_mm;
	uint32_t temp = (uint32_t)(presc * scaled_radius);
	uint16_t downscaled = (uint16_t)(temp / 1000);
	uint8_t res = (downscaled + 5) / 10; // round
	return res;
}


uint16_t calc_temp_freq_Z_for_diagonal_move(uint8_t steps_per_mm_Z)
{
	uint16_t freq = (uint32_t)((uint32_t)(FREQ_STEPPER_X * STEPS_PER_MM_X) * 10)  / steps_per_mm_Z;
	return (freq + 5) / 10;
}


uint8_t calc_pixel_unit_width_mm(uint8_t radius_obj_mm)
{
	uint16_t upscaled_res= PIXEL_CALC_PRESC_UPSCALED_1000 * radius_obj_mm;
	// Always round down
	uint8_t res_floored = upscaled_res / 1000;
	return res_floored;
}

void SMAC_init(StepperMotionAxisController*c, uint8_t object_radius_z_mm_, ObjectType type)
{
	c->object_radius_mm = object_radius_z_mm_;
	c->pixel_unit_mm = calc_pixel_unit_width_mm(object_radius_z_mm_)-1;
	c->steps_per_mm_z = calc_steps_per_mm_Z(object_radius_z_mm_);
	c->state = STATE_IDLE;
	c->objtype = type;
	SMAC_init_XYZ();
	
}
