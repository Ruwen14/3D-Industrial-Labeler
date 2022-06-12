/*
 * XYAxisController.c
 *
 * Created: 03.06.2022 11:11:24
 *  Author: ruwen
 */ 

#include "XYAxisController.h"


void XY_stepper_init(void)
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
	
	// X-Stepper 500 Hz
	OCR3A = 249;
	
	// Y-Stepper 500 Hz
	OCR1A = 249;
}

void XY_start_pwm(void)
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

void XY_stop_pwm(void)
{
	TCCR3B = 0;
	TCCR1B = 0;
	
	TCCR3A = 0;
	TCCR1A = 0;
}


uint16_t XY_calc_ocrna_for_one_shot_max83(uint8_t move_mm)
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

