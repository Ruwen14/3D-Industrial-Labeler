/*
 * controll_buttons.c
 *
 * Created: 11.07.2022 15:21:53
 *  Author: Oli
 */ 


#include "controll_buttons.h"

void controll_buttons_init(void)
{
	// T1 Grüner Knopf
	BitClear(DDRG, VERIFY_BUTTON_PIN);
	
	// Roter Knopf
	BitClear(DDRL, CANCEL_BUTTON_PIN);
}
