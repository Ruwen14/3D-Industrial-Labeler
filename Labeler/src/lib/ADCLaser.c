/*
 * ADCLaser.c
 *
 * Created: 03.06.2022 09:51:01
 *  Author: Oli
 */ 

#include "uart.h"



// ***********************************************************************************************************************
// READ ME BEGIN
// ***********************************************************************************************************************
// Die Dokumentation der Funktionen sowie die deren Nutzung sind dem Header-File "ZAxisController.h" zu entnehmen.
// ***********************************************************************************************************************
// READ ME END
// ***********************************************************************************************************************

void ADC_Laser_init(void)
{
	// ADC Konfiguration
	
	// ADMUX definieren
	// rechtsbündig
	ADMUX &= ~(1<<ADLAR);
	// AVCC als Referenzspannung
	ADMUX |= (1<<REFS0);
	// Wahl des Eingangs PK5
	ADMUX |= (1<<MUX2) | (1<<MUX0);
	
	// MUX5 konfigurieren
	ADCSRB |= (1<<MUX5);
	
	// ADCSRA konfigurieren
	// Wandler einschalten, Prescaler 128
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	// Laser-Sensor 2: PK5 als Eingang
	DDRK &= ~(1<<PK5);


	// erste Wandlung durchführen, dauert länger
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADSC);
	
	// uart_Schnittstelle initialisieren
	uart_init(); // ToDo: wegmachen 
}

uint16_t ADC_Laser_read(void)
{
	// Wandlung starten
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADSC);
	
	// Warte bis fertig
	while(!(ADCSRA & (1<<ADSC)))
	{
		
	}
	// Gibt den aktuellen Wert von ADC aus
	return ADC;
}

uint8_t laser_quantize(uint16_t laser_pos, uint8_t low_laser, uint16_t high_laser, uint8_t quant_low, uint8_t quant_high)
{
	uint16_t temp = (laser_pos - low_laser) * (quant_high - quant_low);
	uint16_t temp2 = (high_laser - low_laser) + quant_low;
	return temp / temp2;
}

uint16_t laser_quantize_10th_mm(uint16_t laser_pos, uint8_t low_laser, uint16_t high_laser, uint8_t quant_low, uint16_t quant_high)
{
	uint32_t temp = (laser_pos - low_laser) * (quant_high - quant_low);
	uint16_t temp2 = (high_laser - low_laser) + quant_low;
	return temp / temp2;
}
