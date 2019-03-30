// 
// 
// 

#include "HotEnd.h"

HotEnd::HotEnd()
{
	this->Kp = KP;
	this->Ki = KI;
	this->Kd = KD;
	this->temperature = 0;
	this->setPoint = 0;
	this->PWMValue = 0;
}

HotEnd::HotEnd(uint8_t * thermistorPins, uint8_t * PWMPins)
{
	HotEnd();
	for (int i = 0; i < 4; i++)
	{
		this->ThermistorPins[i] = thermistorPins[i];
	}
	this->PWMPins[0] = PWMPins[0];
	this->PWMPins[1] = PWMPins[1];
}

void HotEnd::SetDesiredTemp(int & temp)
{

}


