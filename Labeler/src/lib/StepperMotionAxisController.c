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

uint16_t SMAC_calc_one_shot_timer_Z_max233(uint8_t move_mm)
{
	uint8_t pulse_period_ms = 2;

	// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
	uint32_t T_interrupt_period = move_mm * STEPS_PER_MM_Z * pulse_period_ms;

	uint32_t OCRNA_ = (T_interrupt_period * 15625) - 1000;

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
		c->stateflags = 1;
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

	uint16_t oneshot_val = SMAC_calc_one_shot_timer_Z_max233(seq->move_Z);
	
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_Z_pwm();	
	
}

static void _trigger_motion_diagonal(StepperMotionAxisController*c, MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_max82(seq->move_XY);
	
	// Halbiere Frequenz sodass gleicher Weg in gleicher Zeit verfahren wird
	OCR4A = 640;
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_XY_pwm();
	SMAC_run_Z_pwm();
}

static void _trigger_motion_drawing_level(StepperMotionAxisController*c, MotionSequence* seq)
{
	uint16_t laser_data = ADC_Laser_read();
	uint16_t y_dist = laser_quantize_10th_mm(laser_data, 2, 1003, 0, 503);
	
	// Schlitten muss im Messbereich sein!
// 	if (y_dist >= STEPPER_INSIDE_MEAS_RANGE)
// 		return;	

	// Extrahiere mm und 1/10 mm aus y_dist(dezimm). Speicher in uin8_t OK!, da y_dist den 16-Bit Werte-Bereich nicht ausnutzt.
	uint8_t mm = y_dist / 10;
	uint8_t tenth_mm = y_dist - mm * 10;
	
	
	
	uart_send_16bit(y_dist);
	uart_send_16bit(y_dist);
	uart_send_16bit(y_dist);
	uart_send_16bit(y_dist);
	uart_send_16bit(y_dist);
	uart_send_16bit(y_dist);
	uart_send_16bit(y_dist);
	uart_send_16bit(y_dist);
	
	
	
	
	if (y_dist > 5)
	{
		c->mode = MODE_SETUP_DRAWING_LEVEL;
		uint16_t drive = y_dist - 5;
		uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_max82(mm);
	
	
		SMAC_start_one_shot_timer(oneshot_val);
		SMAC_run_XY_pwm();
		
	}
	
// 	else if (mm == 0 && tenth_mm > 5)
// 	{
// 		c->mode = MODE_SETUP_DRAWING_LEVEL;
// 		
// 		uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_10th_mm_max820(tenth_mm);
// 		SMAC_start_one_shot_timer(oneshot_val);
// 		SMAC_run_XY_pwm();
// 	}
	
	// Nah genug dran
	else
	{
		c->mode = MODE_NORMAL;
	}
	
	
	
	
	
}


void SMAC_start_new_motion_sequence(StepperMotionAxisController* c, MotionSequence* seq)
{
	// Wird wahrscheinlich zu einem Jump-Table optimiert.
	// Performance entsprechend O(1) -> const.
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

		default:
		break;
	}
}

