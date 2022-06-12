/*
 * XYAxisController.h
 *
 * Created: 03.06.2022 11:09:09
 *  Author: ruwen
 */ 


#ifndef XYAXISCONTROLLER_H_
#define XYAXISCONTROLLER_H_

#include "constants.h"
#include "movement.h"
#include <avr/io.h>
#include <avr/interrupt.h>

typedef struct  
{
	volatile uint8_t is_moving_X;
	volatile uint8_t is_moving_Y;
} XYAxisController;

void XY_stepper_init(void);

void XY_start_pwm(void);

void XY_stop_pwm(void);

uint16_t XY_calc_ocrna_for_one_shot_max83(uint8_t move_mm);






#endif /* XYAXISCONTROLLER_H_ */