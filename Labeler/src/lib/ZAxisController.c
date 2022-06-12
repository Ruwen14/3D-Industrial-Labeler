/*
 * ZAxisController.c
 *
 * Created: 02.06.2022 09:56:22
 *  Author: ruwen
 */ 

#include "ZAxisController.h"



// ***********************************************************************************************************************
// READ ME BEGIN
// ***********************************************************************************************************************
// Die Dokumentation der Funktionen sowie die deren Nutzung sind dem Header-File "ZAxisController.h" zu entnehmen.
// ***********************************************************************************************************************
// READ ME END
// ***********************************************************************************************************************

void Z_stepper_init(void)
{
	// Ausgang für Z-Achse konfigurieren
	BitSet(DDRH, Z_TAKT);
	BitSet(DDRL, Z_DIR);
	BitSet(DDRL, Z_ACTIVE);
	
	// Z-Schrittmotor aktivieren
	Z_SET_ACTIVE;
	
	
	// 500 Hz
	OCR4A = 249;
}

void Z_start_pwm(void)
{
	// WGM42 CTC Mode | CS41 + CS40 -> prescaler 64
	TCCR4B = (1<<WGM42) | (1<<CS41) | (1<<CS40);
	
	// Reset Counter
	TCNT5 = 0;
	
	// Toggle den Output Pin OC4B = PH4 = Z_TAKT
	TCCR4A |= (1<<COM4B0);
}

void Z_stop_pwm(void)
{
	// No Source (Timer/Counter stopped)
	TCCR4B = 0;
	
	// Wahrscheinlich nicht benötigt ??? ToDo: Ruwen
	TCCR4A = 0;
}


// Die Werte sind zwischenzeitlich um den Faktor 1000 hochskaliert, um eine Auflösung von 1 ms zu garantieren ohne
// auf Gleitkommazahlen auszuweichen. Parameter move_mm wird bereits in den Z_move*...-Operationen auf <=233 kontrolliert.
uint16_t Z_calc_ocrna_for_one_shot_max233(uint8_t move_mm)
{
	// Folgende Operationen sind bestimmt auch als Shifting-Operationen durchzuführen.
	// Da diese Funktion jedoch vor der PWM-Generationen/One-Shot-Start aufgerufen wird
	// und somit nicht zeitkritisch ist, habe ich mir mal die Mühe ersparrt :). Ruwen

	uint8_t pulse_period_ms = 2;

	// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
	uint32_t T_interrupt_period = move_mm * STEPS_PER_MM_Z * pulse_period_ms;

	uint32_t OCRNA_ = (T_interrupt_period * 15625) - 1000;

	uint16_t OCRNA_rescaled_SI = OCRNA_ / 1000;

	return OCRNA_rescaled_SI;
}


void Z_start_one_shot_timer(uint16_t ocrna_)
{
	// OC5B timer
	OCR5B = ocrna_; // 
	
// The counter value (TCNTn) increases until a compare match occurs with either OCRnA or ICRn, and then counter (TCNTn) is cleared.
// PAGE 146
// 	// TEST SECOND CTC
// 	OCR5A = 31000; // Zählt maximal bis ORC5A uns reset
	 
	// Reset da in Z_disable_one_shot_timer potenziell cleared.
	// WGM52 CTC Mode | (1<<CS52) + (1<<CS50) prescaler 1024
	TCCR5B = (1<<WGM52) | (1<<CS52) | (1<<CS50);
	
	// Clear COM5B1 + COM5B0 for disconnecting OC5B. Usable for Normal port operation (p. 155)
	TCCR5A &= ~((1<<COM5B1) | (1<<COM5B0));

	// Reset da in Z_disable_one_shot_timer potenziell cleared.
	// Enable Interrupt on Compare Value OCR5B
	TIMSK5 |= (1<<OCIE5B);
	TIMSK5 |= (1<<OCIE5A);
	
	
	sei();
}

void Z_disable_one_shot_timer(void)
{
	// Deaktiviere Output Compare B Match Interrupt
	TIMSK5 &= ~(1 << OCIE5B);

	// No clock source. (Timer / Counter stopped)
	TCCR5B &= ~((1 << CS51) | (1 << CS52) | (1 << CS50));
}


void Z_move_clockwise_max233(ZAxisController* self, uint8_t move_mm)
{
	// Ändere Drehrichtung
	BitClear(PORTL, Z_DIR);
	
	uint16_t oneshot_ocv = Z_calc_ocrna_for_one_shot_max233(move_mm);
	Z_start_pwm();
	Z_start_one_shot_timer(oneshot_ocv);
}

void Z_move_anticlockwise_max233(ZAxisController* self, uint8_t move_mm)
{
	// Ändere Drehrichtung
	BitSet(PORTL, Z_DIR);

	uint16_t oneshot_ocv = Z_calc_ocrna_for_one_shot_max233(move_mm);
	Z_start_pwm();
	Z_start_one_shot_timer(oneshot_ocv);
}