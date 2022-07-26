/*
 * limit_switches.h
 *
 * Created: 25.05.2022 16:56:58
 *  Author: ruwen & Oli
 */ 


#ifndef LIMIT_SWITCHES_H_
#define LIMIT_SWITCHES_H_

#include "constants.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void limit_swiches_init(void);

void prevent_bounce(void);






#endif /* LIMIT_SWITCHES_H_ */