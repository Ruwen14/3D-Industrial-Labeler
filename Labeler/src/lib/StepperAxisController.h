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

#define DISABLE_TIMER5_COMPA_vect TIMSK5 &= ~(1 << OCIE5A);
#define DISABLE_TIMER5_COMPB_vect TIMSK5 &= ~(1 << OCIE5B);

// Helper Makros um Informationen über Controller in einer uin8_t variable zu encoden
#define SET_FLAG_X_MOVING(Reg) BitSet(Reg, 0)
#define SET_FLAG_Y_MOVING(Reg) BitSet(Reg, 1)
#define SET_FLAG_Z_MOVING(Reg) BitSet(Reg, 2)
#define SET_FLAG_STEPPER_MOVING(Reg) BitSet(Reg, 3)
#define SET_FLAG_DUAL_AXIS_MOVING(Reg) BitSet(Reg, 4)
#define SET_FLAG_Z_USING_OCR5B(Reg) BitSet(Reg, 5)


#define RESET_ALL_AXIS_FLAGS(Reg) Reg = 0
#define RESET_FLAG_X_MOVING(Reg) BitClear(Reg, 0)
#define RESET_FLAG_Y_MOVING(Reg) BitClear(Reg, 1)
#define RESET_FLAG_Z_MOVING(Reg) BitClear(Reg, 2)
#define RESET_FLAG_STEPPER_MOVING(Reg) BitClear(Reg, 3)
#define RESET_FLAG_DUAL_AXIS_MOVING(Reg) BitClear(Reg, 4)
#define RESET_FLAG_Z_USING_OCR5B(Reg) BitClear(Reg, 5)

#define IS_X_MOVING(Reg) IsBitSet(Reg, 0)
#define IS_Y_MOVING(Reg) IsBitSet(Reg, 1)
#define IS_Z_MOVING(Reg) IsBitSet(Reg, 2)
#define IS_STEPPER_MOVING(Reg) IsBitSet(Reg, 3)
#define IS_DUAL_AXIS_MOVING(Reg) IsBitSet(Reg, 4)
#define IS_Z_USING_OCR5B(Reg) IsBitSet(Reg, 5)

typedef volatile struct  
{
	volatile uint8_t stateflags;
// 	int16_t dist_traveled_Z;
// 	int16_t dist_traveled_Y;
// 	int16_t dist_traveled_X;
		
} StepperAxisController;


typedef enum
{
	MOVE_RIGHT,
	MOVE_LEFT
} XAxisDir;

typedef enum
{
	MOVE_UP,
	MOVE_DOWN
} YAxisDir;

typedef enum
{
	ROTATE_CLOCKWISE,
	ROTATE_ANTICLOCKWISE	
} ZAxisDir;


void XYZ_init(void);

void Z_set_dir(ZAxisDir dir);

void Y_set_dir(YAxisDir dir);

void X_set_dir(XAxisDir dir);


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

// INTERNAL-USAGE
// Dieser "One-Shot-Timer" wird genutzt sofern nur eine Bahn X, Y oder Z gefahren wird.
// Bei parallelem Verfahren wird 'start_multi_one_shot_timer' genutzt.
void start_one_shot_timer(uint16_t ocrna_);

// INTERNAL-USAGE
// Disabled den 'Single'-One-Shot-Interrupt. Wird in der ISR aufgerufen
void disable_one_shot_timer(void);


// INTERNAL-USAGE
// Da nur noch einer der insgesamt 4x 16Bit Timer übrig ist, müssen wir diesen für das 
// zeitgleiche Verfahren der XY / Z - Achse über die Register OCR5A / OCR5B aufteilen
// Der "One-Shot-Timer" generiert hierbei 2 COMP-Interrupts. Einmal bei OCR5B und einmal bei
// OCR5A. Da bei OCR5A der Timer TCNT5 zurückgesetzt wird muss OCR5B < OCR5A sein !. (S. 146)
void start_multi_one_shot_timer(uint16_t ocrna_, uint16_t ocrnb_);

// INTERNAL-USAGE
// Disabled den 'Multi'-One-Shot-Interrupt. Wird in der ISR aufgerufen
void disable_multi_one_shot_timer(void);


void Y_move_max82(StepperAxisController* controller, uint8_t move_mm, YAxisDir dir);

void X_move_max82(StepperAxisController* controller, uint8_t move_mm, XAxisDir dir);

void Z_move_max233(StepperAxisController* controller, uint8_t move_mm, ZAxisDir dir);

void ZX_move_parallel(StepperAxisController* controller, uint8_t z_move_mm, ZAxisDir zdir, uint8_t x_move_mm, XAxisDir xdir);

void ZY_move_parallel(StepperAxisController* controller, uint8_t z_move_mm, ZAxisDir zdir, uint8_t y_move_mm, YAxisDir ydir);

void ZX_move_diagonal_45d(StepperAxisController* controller, uint8_t heightWidth_mm, ZAxisDir zdir,  XAxisDir xdir);











#endif /* STEPPERAXISCONTROLLER_H_ */