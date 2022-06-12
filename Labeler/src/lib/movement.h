/*
 * movement.h
 *
 * Created: 20.05.2022 11:55:08
 *  Author: ruwen
 */ 


#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "constants.h"
#include <avr/io.h>
#include <util/delay.h>

// Aliase für Ansteuerung
#define Z_RISING_EDGE PORTH |= (1<<Z_TAKT)
#define Z_FALLING_EDGE PORTH &= ~(1<<Z_TAKT)

#define XY_RISING_EDGE \
PORTE |= (1<<X_TAKT); \
PORTB |= (1<<Y_TAKT);

#define XY_FALLING_EDGE \
PORTE &= ~(1<<X_TAKT); \
PORTB &= ~(1<<Y_TAKT);



#define Z_SET_ACTIVE BitSet(PORTL, Z_ACTIVE);
#define Z_SET_INACTIVE BitClear(PORTL, Z_ACTIVE);

#define XY_SET_ACTIVE \
BitClear(PORTA, X_ACTIVE); \
BitClear(PORTA, Y_ACTIVE);

#define XY_SET_INACTIVE \
BitSet(PORTA, X_ACTIVE); \
BitSet(PORTA, Y_ACTIVE);

void zaxis_init(void);
void zaxis_move_clockwise(uint16_t steps);
void zaxis_move_anticlockwise(uint16_t steps);

void xyaxis_init(void);
void xyaxis_move_right(uint16_t steps);
void xyaxis_move_left(uint16_t steps);
void xyaxis_move_up(uint16_t steps);
void xyaxis_move_down(uint16_t steps);









#endif /* MOVEMENT_H_ */