/*
 * LCD.c
 *
 * Created: 03.06.2022 11:39:45
 *  Author: Oli
 */ 

# include "LCD.h"



// ***********************************************************************************************************************
// READ ME BEGIN
// ***********************************************************************************************************************
// Die Dokumentation der Funktionen sowie die deren Nutzung sind dem Header-File "ZAxisController.h" zu entnehmen.
// ***********************************************************************************************************************
// READ ME END
// ***********************************************************************************************************************

void lcd_init()
{
	DDRC |= (OHB + ENABLE + RS);		// Ausg�nge PC2 bis PC7 setzen
	_delay_ms(20);						// Wartezeit nach PowerUp
	PORTC &= (ENABLE + RS232BITS);		// OHB=0 und RS=0 setzen
	PORTC |= (LCD_RESET + ENABLE);		// LCD-Reset und E=1 setzen
	PORTC &= ~ENABLE;					// E=0 setzen
	_delay_ms(5);						// Befehlsausf�hrung
	PORTC |= ENABLE;					// E=1 setzen
	PORTC &= ~ENABLE;					// E=0 setzen
	_delay_ms(1);						// Befehlsausf�hrung
	PORTC |= ENABLE;					// E=1 setzen
	PORTC &= ~ENABLE;					// E=0 setzen
	_delay_ms(1);						// Befehlsausf�hrung
	PORTC |= ENABLE;					// E=1 setzen
	PORTC &= ~OHB;						// OHB in PORTD l�schen
	PORTC |= LCD_INTERFACE;				// 4-Bit Interface setzen
	PORTC &= ~ENABLE;					// E=0 setzen
	_delay_ms(1);						// Befehlsausf�hrung
	PORTC |= ENABLE;					// E=1 setzen

	// ab hier ist das 4-Bit Interface aktiv
	lcd_cmd(0x28);						// 4-Bit Interface, 2 zeilig, 5x7 Character
	lcd_cmd(0x06);						// Cursor increase, no display shift
	lcd_cmd(0x0F);						// Display on
	lcd_cmd(0x01);						// Display clear
	_delay_ms(1);						// Befehlsausf�hrung
}

void lcd_cmd(unsigned char cmd)
{
	PORTC &= (ENABLE + RS232BITS);		// OHB=0 und RS=0 setzen
	PORTC |= ENABLE;					// E=1 setzen
	PORTC |= (cmd & OHB);				// Einsen aus OHB von cmd �bernehmen
	PORTC &= ~ENABLE;					// E=0 setzen
	PORTC |= ENABLE;					// E=1 setzen
	PORTC &= ~OHB;						// OHB in PORTC l�schen
	PORTC |= (cmd<<4);					// UHB von cmd in OHB von PORTD schreiben
	PORTC &= ~ENABLE;					// E=0 setzen
	_delay_ms(1);						// Befehlsausf�hrung
	return;
}

void lcd_zahl (uint8_t zahl, char* text)
{
	char ziff1;							// Hunderterstelle
	char ziff2;							// Zehnerstelle
	
	ziff1 = zahl/100;					// Hunderterstelle ermitteln
	zahl -= ziff1 * 100;				// Hunderter abziehen
	
	ziff2 =	zahl/10;					// Zehnerstelle ermitteln
	zahl -= ziff2 * 10;					// Zehner abziehen
	
	text[0] = ziff1 + 0x30;				// ASCII-Code f�r Hunderter
	text[1] = ziff2 + 0x30;				// ASCII-Code f�r Zehner
	text[2] = zahl + 0x30;				// ASCII-Code f�r Einer
	text[3] = 0x00;						// Endekennung
	return;
}




void lcd_text(char* ztext)
{
	uint8_t j = 0;						// Z�hlvariable initialisieren
	
	while (ztext[j] != 0)
	{
		PORTC &= (~OHB);				// OHB=0 setzen
		PORTC |= RS;					// RS=1 setzen
		PORTC |= ENABLE;				// E=1 setzen
		PORTC |= (ztext[j] & OHB);		// Einsen aus OHB von ztext �bernehmen
		PORTC &= ~ENABLE;				// E=0 setzen
		PORTC |= ENABLE;				// E=1 setzen
		PORTC &= ~OHB;					// OHB in PORTD l�schen

		PORTC |= (ztext[j]<<4);			// UHB von ztext in OHB von PORTD schreiben
		PORTC &= ~ENABLE;				// E=0 setzen
		j++;							// j inkrementieren
		_delay_ms(1);					// Befehlsausf�hrung
		if (j==16)
		{
			lcd_cmd(0xc0);
		}
	}
	return;
}