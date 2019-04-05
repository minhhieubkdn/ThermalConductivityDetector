// 
// 
// 

#include "Themistor.h"

Thermistor::Thermistor(uint8_t thermPin)
{
	this->therm = thermistor(thermPin, 0);
}

void Thermistor::Read()
{
	this->floatTemp = therm.analog2temp();
	this->temperature = uint16_t(this->floatTemp);
}

uint16_t Thermistor::ReadTemp()
{
	Read();
	return this->temperature;
}

float Thermistor::ReadTempFloat()
{
	Read();
	return this->floatTemp;
}
