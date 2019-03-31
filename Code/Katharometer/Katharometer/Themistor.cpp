// 
// 
// 

#include "Themistor.h"

Thermistor::Thermistor(uint8_t thermPin, uint8_t type)
{
	this->therm = thermistor(thermPin, type);
}

void Thermistor::Read()
{
	this->doubleTemp = therm.analog2temp();
	this->temperature = uint16_t(this->doubleTemp);
}

uint16_t Thermistor::ReadTemp()
{
	Read();
	return this->temperature;
}

double Thermistor::ReadTempDouble()
{
	Read();
	return this->doubleTemp;
}
