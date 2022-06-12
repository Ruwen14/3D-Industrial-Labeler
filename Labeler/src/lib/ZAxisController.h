/*
 * ZAxisController.h
 *
 * Created: 02.06.2022 09:45:04
 *  Author: ruwen
 */ 


#ifndef ZAXISCONTROLLER_H_
#define ZAXISCONTROLLER_H_

#include "constants.h"
#include "movement.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// ***********************************************************************************************************************
// PUBLIC-API BEGIN
// ***********************************************************************************************************************

// PUBLIC-API
// ZAxisController-Klasse. Synchronisiert, steuert und �berwacht Fahrtabl�ufe des Z-Achsen-Steppers.
typedef struct  
{
	volatile uint8_t is_moving_Z;
} ZAxisController;


// PUBLIC-API
// Initialisiert die ben�tigte Konfiguration des Stepper-Motors f�r 500 Hz 1/16-Schrittbetrieb
void Z_stepper_init(void);

// PUBLIC-API
// L�sst den Stepper um den �bergebenen Verfahrweg in mm im Uhrzeigersinn drehen. 
// Die Stepper-Motor-Fahrt ist komplett hardware basiert und generiert 
// maximal nur einen Interrupt pro Verfahrweg. Der maximal �bergebene Verfahrweg darf nicht 233 mm �berschreiten.
void Z_move_clockwise_max233(ZAxisController* self, uint8_t move_mm);

// PUBLIC-API
// L�sst den Stepper um den �bergebenen Verfahrweg in mm im Gegen den Uhrzeigersinn drehen. 
// Die Stepper-Motor-Fahrt ist komplett hardware basiert und generiert 
// maximal nur einen Interrupt pro Verfahrweg. Der maximal �bergebene Verfahrweg darf nicht 233 mm �berschreiten.
void Z_move_anticlockwise_max233(ZAxisController* self, uint8_t move_mm);

// ***********************************************************************************************************************
// PUBLIC-API END
// ***********************************************************************************************************************

//------------------------------------------------------------------------------------------------------------------------

// ***********************************************************************************************************************
// INTERNAL-API BEGIN. DONT CALL DIRECTLY!
// ***********************************************************************************************************************

// INTERNAL-USAGE
// Startet einen 500 Hz Rechteckgenerator, der automatisch nach verfahrenen Weg deaktiviert wird
void Z_start_pwm(void);

// INTERNAL-USAGE
// Stopt Rechteckgenerator. Wird vom One-Shot-Interrupt aufgerufen.
void Z_stop_pwm(void);


// INTERNAL-USAGE
// Berechnet den Output-Compare-Value f�r einen One-Shot-Interrupt mit einer Aufl�sung von 1 ms, 
// der die Stepper-PWM nach verfahrenen Weg stoppt. Maximalwert von 233 darf nicht �berschritten werden,
// da sonst der Output-Compare-Value (16 bit) �berl�uft.
uint16_t Z_calc_ocrna_for_one_shot_max233(uint8_t move_mm);

// INTERNAL-USAGE
// Startet einen One-Shot-Interrupt, der die PWM f�r den Steppermotor 
// nach verfahrenen Steps deaktiviert. 
void Z_start_one_shot_timer(uint16_t ocrna_);

// INTERNAL-USAGE
// Disabled den One-Shot-Interrupt. Wird in der ISR aufgerufen
void Z_disable_one_shot_timer(void);


// ***********************************************************************************************************************
// INTERNAL-API END. DONT CALL DIRECTLY!
// ***********************************************************************************************************************


#endif /* ZAXISCONTROLLER_H_ */
