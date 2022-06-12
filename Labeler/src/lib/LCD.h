/*
 * LCD.h
 *
 * Created: 03.06.2022 11:46:15
 *  Author: Oli
 */ 


#ifndef LCD_H_
#define LCD_H_

#include "constants.h"
#include <util/delay.h>


// Initialisiert das LCD-Display und nimmt einige grundlegende Konfigurationen vor
// 4-Bit Interface, 2 zeilig, 5x7 Character, Cursor increase, no display shift, Display on, Display clear
void lcd_init();

// Befehl zur direkten Angabe von Befehlen an das Display
void lcd_cmd(unsigned char cmd);

// Umwandlung von Zahlen in char
void lcd_zahl (uint8_t zahl, char* text);

// Befehl zur Ausgabe von Text auf dem Display
void lcd_text(char* ztext);


#endif /* LCD_H_ */