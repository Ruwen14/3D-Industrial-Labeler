/*
 * StepperAxisController.h
 *
 * Created: 12.06.2022 16:01:01
 *  Author: ruwen
 */ 


#ifndef STEPPERAXISCONTROLLER_H_
#define STEPPERAXISCONTROLLER_H_

#include "constants.h"
#include "movement.h"
#include <avr/io.h>
#include <avr/interrupt.h>

typedef struct  
{
	volatile uint8_t is_moving_Z;
	volatile uint8_t is_moving_X;
	volatile uint8_t is_moving_Y;
} StepperAxisController;


enum XAxisDir
{
	MOVE_RIGHT,
	MOVE_LEFT
};

enum YAxisDir
{
	MOVE_UP,
	MOVE_DOWN
};

enum ZAxisDir
{
	ROTATE_CLOCKWISE,
	ROTATE_ANTICLOCKWISE	
};




void XYZ_init(void);

void Z_run_pwm(void);

void Z_disable_pwm(void);

void XY_run_pwm(void);

void XY_disable_pwm(void);


// INTERNAL-USAGE
// Berechnet den Output-Compare-Value für einen One-Shot-Interrupt mit einer Auflösung von 1 ms,
// der die Stepper-PWM nach verfahrenen Weg stoppt. Maximalwert von 233 [mm] darf nicht überschritten werden,
// da sonst der Output-Compare-Value (16 bit) überläuft.
uint16_t Z_calc_one_shot_timer_max233(uint8_t move_mm);


// INTERNAL-USAGE
// Berechnet den Output-Compare-Value für einen One-Shot-Interrupt mit einer Auflösung von 1 ms,
// der die Stepper-PWM nach verfahrenen Weg stoppt. Maximalwert von 82 [mm] darf nicht überschritten werden,
// da sonst der Output-Compare-Value (16 bit) überläuft.
uint16_t XY_calc_one_shot_timer_max82(uint8_t move_mm);









void Y_move_max82(StepperAxisController* controller, uint8_t move_mm, YAxisDir dir);

void X_move_max82(StepperAxisController* controller, uint8_t move_mm, XAxisDir dir);

void Z_move_max233(StepperAxisController* controller, uint8_t move_mm, ZAxisDir dir);
















#endif /* STEPPERAXISCONTROLLER_H_ */