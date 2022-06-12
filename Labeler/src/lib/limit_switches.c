/*
 * limit_switches.c
 *
 * Created: 25.05.2022 16:58:08
 *  Author: ruwen
 */ 

#include "limit_switches.h"

void limit_swiches_init(void)
{
	// PD2 als Eingang
	BitClear(DDRD, X_LEFT_LIM_PIN);
	// PD0 als Eingang
	BitClear(DDRD, X_RIGHT_LIM_PIN);
	// PD3 als Eingang
	BitClear(DDRD, Y_BOTTOM_LIM_PIN);
	// PD1 als Eingang
	BitClear(DDRD, Y_TOP_LIM_PIN);
	
	// External Hardware Interrupts konfigurieren
	// INT0-INT3 ausschalten solange wir es konfigurieren (Empfehlung Datenblatt)
	BitClear(EIMSK, INT2);
	BitClear(EIMSK, INT0);
	BitClear(EIMSK, INT3);
	BitClear(EIMSK, INT1);
	
	// EICRA ist verantwortlich für INT0-INT3
	// INT0-INT3 soll bei jeder Flankenänderung getriggered werden,
	// um immer aktuellen Schalter-Zustand zu haben
	// -------------------------------------------------------------
	BitClear(EICRA, ISC21);
	BitSet(EICRA, ISC20);
	
	BitClear(EICRA, ISC01);
	BitSet(EICRA, ISC00);
	
	BitClear(EICRA, ISC31);
	BitSet(EICRA, ISC30);
	
	BitClear(EICRA, ISC11);
	BitSet(EICRA, ISC10);
	// -------------------------------------------------------------
	
	
	// INT0-INT3 wieder anschalten
	BitSet(EIMSK, INT2);
	BitSet(EIMSK, INT0);
	BitSet(EIMSK, INT3);
	BitSet(EIMSK, INT1);
	
	// Set-Enable-Bit. I-Bit im Statusregister setzten
	sei();
	
	
}
