/*
 * controll_buttons.h
 *
 * Created: 08.07.2022 16:48:12
 *  Author: ruwen
 */ 


#ifndef CONTROLL_BUTTONS_H_
#define CONTROLL_BUTTONS_H_

#include "constants.h"
#include <avr/io.h>

#define IS_VERIFY_BUTTON_PRESSED (IsBitSet(PING, VERIFY_BUTTON_PIN))

#define IS_CANCEL_BUTTON_PRESSED (IsBitSet(PINL, CANCEL_BUTTON_PIN))

void controll_buttons_init(void);









#endif /* CONTROLL_BUTTONS_H_ */