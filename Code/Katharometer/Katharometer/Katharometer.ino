/*
 Name:		Katharometer.ino
 Created:	3/30/2019 3:20:17 PM
 Author:	Minh Hieu
*/

#include "Themistor.h"
#include "HotEnd.h"

uint8_t midThermistorPins[] = { A0, A2 };
uint8_t outThermistorPins[] = { A1, A3 };
uint8_t PWMPins[] = { 10, 11 };

HotEnd Heater = HotEnd(midThermistorPins, PWMPins);
Thermistor OutLeft = Thermistor(outThermistorPins[0], 0);
Thermistor OutRight = Thermistor(outThermistorPins[1], 0);

uint16_t DesiredMidTemp;
uint16_t CurrentMidTemps[2];
uint16_t CurrentOutTemps[2];

void setup() {
	DesiredMidTemp = 100;
	Heater.SetDesiredTemp(DesiredMidTemp);
}

void loop() {
	Heater.Execute();
}
