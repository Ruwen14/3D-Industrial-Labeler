/*
 * Test.h
 *
 * Created: 11.07.2022 22:19:15
 *  Author: ruwen
 */ 


#ifndef TEST_H_
#define TEST_H_
#define RISING_EDGE 1;
#define FALLING_EDGE 1;



void stumpf_und_simpel(uint16_t steps)
{
	for (uint16_t i = 0; i < steps; ++i)
	{
		RISING_EDGE;
		_delay_us(1000);
		FALLING_EDGE;
		_delay_us(1000);
	}
}

void disable_interrupt(void);

volatile uint16_t steps_left;


ISR(TIMER0_COMPA_vect)
{	
	RISING_EDGE;
	// toggle ...
	FALLING_EDGE;
	
	steps_left--;
	if (steps_left == 0)
	{
		disable_interrupt();
	}
}




#endif /* TEST_H_ */