/*
 * ADCLaser.h
 *
 * Created: 03.06.2022 09:51:18
 *  Author: Oli
 */ 


#ifndef ADCLASER_H_
#define ADCLASER_H_

#include "constants.h"
#include <avr/io.h>
#include <util/delay.h>

#define MEDIAN_FILTER_SIZE 15 // Muss ungerade sein


// Initialisiert die benötigte Konfiguration des ADC-Wandlers für die Nutzung mit dem Laser-Sensor 2
void ADC_Laser_init(void);

// Startet eine Wandlung und gibt anschließend den aktuellen Wert von ADC aus
uint16_t ADC_Laser_read(void);

uint8_t laser_quantize(uint16_t laser_pos, uint8_t low_laser, uint16_t high_laser, uint8_t quant_low, uint8_t quant_high);

uint16_t laser_quantize_10th_mm(uint16_t laser_pos, uint8_t low_laser, uint16_t high_laser, uint8_t quant_low, uint16_t quant_high);

uint16_t ADC_Laser_read_mean(void);

uint16_t ADC_Laser_read_median(void);

uint16_t median_filter(uint16_t* value_buf);




#endif /* ADCLASER_H_ */