/*
 * movement.c
 *
 * Created: 20.05.2022 11:57:27
 *  Author: ruwen
 */ 

#include "movement.h"


// ---------------- Z-Schrittmotor --------------------
void zaxis_init(void)
{
	// Ausgang für Z-Achse konfigurieren
	BitSet(DDRH, Z_TAKT);
	BitSet(DDRL, Z_DIR);
	BitSet(DDRL, Z_ACTIVE);
	
	// Z-Schrittmotr aktivieren
	Z_SET_ACTIVE;
}

void zaxis_move_clockwise(uint16_t steps)
{
	// Change Direction
	BitClear(PORTL, Z_DIR);
	
	for (uint16_t i = 0; i < steps; ++i )
	{
		Z_RISING_EDGE;
		_delay_us(500);
		Z_FALLING_EDGE;
		_delay_us(500);
	}
}

void zaxis_move_anticlockwise(uint16_t steps)
{
	// Change Direction
	BitSet(PORTL, Z_DIR);

	for (uint16_t i = 0; i < steps; ++i )
	{
		Z_RISING_EDGE;
		_delay_us(950);
		Z_FALLING_EDGE;
		_delay_us(950);
	}
}
// ----------------------------------------------------

// ---------------- XY-Schrittmotor --------------------
void xyaxis_init(void)
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
}


void xyaxis_move_right(uint16_t steps)
{
	// Direction to Right
	BitSet(PORTA, Y_DIR);
	BitSet(PORTA, X_DIR);
	
	for (uint16_t i = 0; i < steps; ++i )
	{
		XY_RISING_EDGE;
		_delay_us(1000);
		XY_FALLING_EDGE;
		_delay_us(1000);
	}
}
void xyaxis_move_left(uint16_t steps)
{
	// Direction to Left
	BitClear(PORTA, Y_DIR);
	BitClear(PORTA, X_DIR);
	
	for (uint16_t i = 0; i < steps; ++i )
	{
		XY_RISING_EDGE;
		_delay_us(1000);
		XY_FALLING_EDGE;
		_delay_us(1000);
	}
}

void xyaxis_move_up(uint16_t steps)
{
	// Direction to Left
	BitSet(PORTA, Y_DIR);
	BitClear(PORTA, X_DIR);
	
	for (uint16_t i = 0; i < steps; ++i )
	{
		XY_RISING_EDGE;
		_delay_us(1500);
		XY_FALLING_EDGE;
		_delay_us(1500);
	}
}

void xyaxis_move_down(uint16_t steps)
{
	// Direction to Left
	BitClear(PORTA, Y_DIR);
	BitSet(PORTA, X_DIR);
	
	for (uint16_t i = 0; i < steps; ++i )
	{
		XY_RISING_EDGE;
		_delay_us(1500);
		XY_FALLING_EDGE;
		_delay_us(1500);
	}
	
}