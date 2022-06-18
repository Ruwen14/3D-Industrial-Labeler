/*
 * StepperAxisController.c
 *
 * Created: 12.06.2022 16:03:18
 *  Author: ruwen
 */ 

#include "StepperAxisController.h"
#include "uart.h"

void XYZ_init(void)
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

void Z_set_dir(ZAxisDir dir)
{
	if (dir == ROTATE_ANTICLOCKWISE) {
		BitSet(PORTL, Z_DIR);
	}
	
	else if (dir == ROTATE_CLOCKWISE) {
		BitClear(PORTL, Z_DIR);
	}
	
	else {
		return;
	}
}

void Y_set_dir(YAxisDir dir)
{
	if (dir == MOVE_UP) {
		BitSet(PORTA, Y_DIR);
		BitClear(PORTA, X_DIR);
	}
	
	else if (dir == MOVE_DOWN) {
		BitClear(PORTA, Y_DIR);
		BitSet(PORTA, X_DIR);
	}
	
	else {
		return;
	}
}

void X_set_dir(XAxisDir dir)
{
	if (dir == MOVE_LEFT) {
		BitClear(PORTA, Y_DIR);
		BitClear(PORTA, X_DIR);
	}
	
	else if (dir == MOVE_RIGHT) {
		BitSet(PORTA, Y_DIR);
		BitSet(PORTA, X_DIR);
	}
	else {
		return;
	}
}

void Z_run_pwm(void)
{
	// WGM42 CTC Mode | CS41 + CS40 -> prescaler 64
	TCCR4B = (1<<WGM42) | (1<<CS41) | (1<<CS40);
	
	// Reset Counter
	TCNT4 = 0;
	
	// Toggle den Output Pin OC4B = PH4 = Z_TAKT
	TCCR4A |= (1<<COM4B0);
}

void Z_disable_pwm(void)
{
	// No Source (Timer/Counter stopped)
	TCCR4B = 0;
	
	// Wahrscheinlich nicht benötigt ??? ToDo: Ruwen
	TCCR4A = 0;
}

void XY_run_pwm(void)
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

void XY_disable_pwm(void)
{
	TCCR3B = 0;
	TCCR1B = 0;
	
	TCCR3A = 0;
	TCCR1A = 0;
}

// Die Werte sind zwischenzeitlich um den Faktor 1000 hochskaliert, um eine Auflösung von 1 ms zu garantieren ohne
// auf Gleitkommazahlen auszuweichen. Parameter move_mm wird bereits in den Z_move*...-Operationen auf <=233 kontrolliert.
uint16_t Z_calc_one_shot_timer_max233(uint8_t move_mm)
{
	uint8_t pulse_period_ms = 2;

	// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
	uint32_t T_interrupt_period = move_mm * STEPS_PER_MM_Z * pulse_period_ms;

	uint32_t OCRNA_ = (T_interrupt_period * 15625) - 1000;

	uint16_t OCRNA_rescaled_SI = OCRNA_ / 1000;

	return OCRNA_rescaled_SI;
}

// Die Werte sind zwischenzeitlich um den Faktor 1000 hochskaliert, um eine Auflösung von 1 ms zu garantieren ohne
// auf Gleitkommazahlen auszuweichen. Parameter move_mm wird bereits in den *_move*...-Operationen auf <=82 kontrolliert.
uint16_t XY_calc_one_shot_timer_max82(uint8_t move_mm)
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

void start_one_shot_timer(uint16_t ocrna_)
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

void disable_one_shot_timer(void)
{
	// DISABLES Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 &= ~(1 << OCIE5A);
	
	// No clock source. (Timer / Counter stopped)
	TCCR5B &= ~((1 << CS51) | (1 << CS52) | (1 << CS50));
}

void start_multi_one_shot_timer(uint16_t ocrna_, uint16_t ocrnb_)
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

void disable_multi_one_shot_timer(void)
{
	// DISABLES Output Compare Match Interrupt 'TIMER5_COMPB_vect'
	TIMSK5 &= ~(1 << OCIE5B);
	// DISABLES Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 &= ~(1 << OCIE5A);
	
	// No clock source. (Timer / Counter stopped)
	TCCR5B &= ~((1 << CS51) | (1 << CS52) | (1 << CS50));
}

void Y_move_max82(StepperAxisController* controller, uint8_t move_mm, YAxisDir dir)
{
	Y_set_dir(dir);
	
	uint16_t oneshot_val = XY_calc_one_shot_timer_max82(move_mm);
	RESET_ALL_AXIS_FLAGS(controller->stateflags);
	SET_FLAG_Y_MOVING(controller->stateflags);
	SET_FLAG_STEPPER_MOVING(controller->stateflags);
	start_one_shot_timer(oneshot_val);
	XY_run_pwm();
	
}

void X_move_max82(StepperAxisController* controller, uint8_t move_mm, XAxisDir dir)
{
	X_set_dir(dir);
	
	uint16_t oneshot_val = XY_calc_one_shot_timer_max82(move_mm);
	
	// compiler will collapse in one instructrion
	RESET_ALL_AXIS_FLAGS(controller->stateflags);
	SET_FLAG_X_MOVING(controller->stateflags);
	SET_FLAG_STEPPER_MOVING(controller->stateflags);
	start_one_shot_timer(oneshot_val);
	XY_run_pwm();

}

void Z_move_max233(StepperAxisController* controller, uint8_t move_mm, ZAxisDir dir)
{
	Z_set_dir(dir);
	
	uint16_t oneshot_val = Z_calc_one_shot_timer_max233(move_mm);
	
	// compiler will collapse in one instructrion
	RESET_ALL_AXIS_FLAGS(controller->stateflags);
	SET_FLAG_Z_MOVING(controller->stateflags);
	SET_FLAG_STEPPER_MOVING(controller->stateflags);
	start_one_shot_timer(oneshot_val);
	Z_run_pwm();
}

void ZX_move_parallel(StepperAxisController* controller, uint8_t z_move_mm, ZAxisDir zdir, uint8_t x_move_mm, XAxisDir xdir)
{
	Z_set_dir(zdir);
	X_set_dir(xdir);
	
	uint16_t z_oneshot_val = Z_calc_one_shot_timer_max233(z_move_mm);
	uint16_t x_oneshot_val = XY_calc_one_shot_timer_max82(x_move_mm);
	
	
	// compiler will collapse in one instructrion
	RESET_ALL_AXIS_FLAGS(controller->stateflags);
	SET_FLAG_DUAL_AXIS_MOVING(controller->stateflags);
	SET_FLAG_Z_MOVING(controller->stateflags);
	SET_FLAG_X_MOVING(controller->stateflags);
	
// 	// Parameter OCRNA has to be bigger than OCRNB
	if (z_oneshot_val > x_oneshot_val) {
		start_multi_one_shot_timer(z_oneshot_val, x_oneshot_val);
	}
	
	else {
		SET_FLAG_Z_USING_OCR5B(controller->stateflags);
		start_multi_one_shot_timer(x_oneshot_val, z_oneshot_val);
	}

	
	Z_run_pwm(); 	
	XY_run_pwm();
}

void ZY_move_parallel(StepperAxisController* controller, uint8_t z_move_mm, ZAxisDir zdir, uint8_t y_move_mm, YAxisDir ydir)
{
	Z_set_dir(zdir);
	Y_set_dir(ydir);
	
	uint16_t z_oneshot_val = Z_calc_one_shot_timer_max233(z_move_mm);
	uint16_t y_oneshot_val = XY_calc_one_shot_timer_max82(y_move_mm);
	
	// compiler will collapse in one instructrion
	RESET_ALL_AXIS_FLAGS(controller->stateflags);
	SET_FLAG_DUAL_AXIS_MOVING(controller->stateflags);
	SET_FLAG_Z_MOVING(controller->stateflags);
	SET_FLAG_Y_MOVING(controller->stateflags);
	
	if (z_oneshot_val > y_oneshot_val) {
		start_multi_one_shot_timer(z_oneshot_val, y_oneshot_val);
	}
	
	else {
		SET_FLAG_Z_USING_OCR5B(controller->stateflags);
		start_multi_one_shot_timer(y_oneshot_val, z_oneshot_val);
	}
	
	Z_run_pwm();
	XY_run_pwm();
}

void ZX_move_diagonal_45d(StepperAxisController* controller, uint8_t heightWidth_mm, ZAxisDir zdir, XAxisDir xdir)
{
	Z_set_dir(zdir);
	X_set_dir(xdir);
	
	uint16_t oneshot_val = XY_calc_one_shot_timer_max82(heightWidth_mm);
	
	
	
	
	// compiler will collapse in one instructrion
	RESET_ALL_AXIS_FLAGS(controller->stateflags);
	SET_FLAG_DUAL_AXIS_MOVING(controller->stateflags);
	SET_FLAG_Z_MOVING(controller->stateflags);
	SET_FLAG_X_MOVING(controller->stateflags);
	
	OCR4A = 640;
	start_one_shot_timer(oneshot_val);;
	Z_run_pwm();
	XY_run_pwm();
}
	
