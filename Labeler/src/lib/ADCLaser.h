/*
 * ADCLaser.h
 *
 * Created: 03.06.2022 09:51:18
 *  Author: Oli
 */ 


#ifndef ADCLASER_H_
#define ADCLASER_H_

#include <avr/io.h>


// Initialisiert die benötigte Konfiguration des ADC-Wandlers für die Nutzung mit dem Laser-Sensor 2
void ADC_Laser_init(void);

// Startet eine Wandlung und gibt anschließend den aktuellen Wert von ADC aus
uint16_t ADC_Laser_read(void);


#endif /* ADCLASER_H_ */