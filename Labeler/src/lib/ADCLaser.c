/*
 * ADCLaser.c
 *
 * Created: 03.06.2022 09:51:01
 *  Author: Oli
 */ 

#include "ADCLaser.h"

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
	
	
}

uint16_t ADC_Laser_read(void)
{
	// Wandlung starten
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADSC); // ToDo 
	
// 	Warte bis fertig
	while(ADCSRA & (1<<ADSC));
	
	// Gibt den aktuellen Wert von ADC aus
	return ADC;
}

// ADC-Wert in mm umrechnen
uint8_t laser_quantize(uint16_t laser_pos, uint8_t low_laser, uint16_t high_laser, uint8_t quant_low, uint8_t quant_high)
{
	uint16_t temp = (laser_pos - low_laser) * (quant_high - quant_low);
	uint16_t temp2 = (high_laser - low_laser) + quant_low;
	return temp / temp2;
}

// ADC-Wert in mm umrechnen. Skaliert für 1/10 mm Genaugigkeit
uint16_t laser_quantize_10th_mm(uint16_t laser_pos, uint8_t low_laser, uint16_t high_laser, uint8_t quant_low, uint16_t quant_high)
{
	 if (laser_pos < low_laser)
	 {
		 return quant_low;
	 }
	     
	else if (laser_pos > high_laser)
	{
		return quant_high;
	}

	uint32_t temp = (uint32_t)(laser_pos - low_laser) * (quant_high - quant_low);
	uint16_t temp2 = (high_laser - low_laser) + quant_low;
	return temp / temp2;
}

// Gemittelter Laser-Mess-Wert über 16 Messungen
uint16_t ADC_Laser_read_mean(void)
{
	uint16_t sum = 0;
	for (uint8_t i = 0; i < 16; i++)
	{
		uint16_t laser_data = ADC_Laser_read();
		sum = sum + laser_quantize_10th_mm(laser_data, LASER_DIGITAL_LOW, LASER_DIGITAL_HIGH, LASER_MEAS_RANGE_LOW, LASER_MEAS_RANGE_HIGH);
	}
	
	// Entspricht round(Summe / 16) im Scaled-Integer-Bereich.
	return ((sum + 8) >> 4);
}

// Median Laser-Messwert über MEDIAN_FILTER_SIZE Messungen
uint16_t ADC_Laser_read_median(void)
{
	
	uint16_t buffer[MEDIAN_FILTER_SIZE];
	
	for (uint8_t i = 0; i < MEDIAN_FILTER_SIZE; i++)
	{
		uint16_t laser_data = ADC_Laser_read();
		buffer[i] = laser_quantize_10th_mm(laser_data, LASER_DIGITAL_LOW, LASER_DIGITAL_HIGH, LASER_MEAS_RANGE_LOW, LASER_MEAS_RANGE_HIGH);
	}
	
	// Entspricht round(Summe / 16) im Scaled-Integer-Bereich.
	return median_filter(buffer);
}



// Simpler Median-Filter nach dem Prinzip: Sortiere und nimm Mitte.
uint16_t median_filter(uint16_t* value_buf)
{
	uint8_t i;
	uint8_t j;
	uint16_t temp;
	for (j = 0; j < MEDIAN_FILTER_SIZE - 1; j++)
	{
		for (i = 0; i < MEDIAN_FILTER_SIZE - j - 1; i++)
		{
			if (value_buf[i] > value_buf[i + 1])
			{
				temp = value_buf[i];
				value_buf[i] = value_buf[i + 1];
				value_buf[i + 1] = temp;
			}
		}
	}
	return value_buf[(MEDIAN_FILTER_SIZE - 1) >> 1];
}

// Berechne Radius, aus Laser-Messwert ausgehend von Messposition
uint8_t calc_radius(uint16_t laser_dist)
{
	int16_t delta = (int16_t)(REFERENCE_DIST - laser_dist);
	uint16_t radius = (uint16_t)(REFERENCE_RAD + delta);
	
	uint8_t rad = (radius + 5) / 10;
	return rad;
}