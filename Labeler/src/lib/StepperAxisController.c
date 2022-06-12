/*
 * StepperAxisController.c
 *
 * Created: 12.06.2022 16:03:18
 *  Author: ruwen
 */ 

#include "StepperAxisController.h"

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

void Z_run_pwm(void)
{
	// WGM42 CTC Mode | CS41 + CS40 -> prescaler 64
	TCCR4B = (1<<WGM42) | (1<<CS41) | (1<<CS40);
	
	// Reset Counter
	TCNT5 = 0;
	
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
// auf Gleitkommazahlen auszuweichen. Parameter move_mm wird bereits in den Z_move*...-Operationen auf <=82 kontrolliert.
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
