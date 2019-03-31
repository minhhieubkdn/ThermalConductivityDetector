// 
// 
// 

#include "HotEnd.h"

HotEnd::HotEnd()
{
	this->Kp = KP;
	this->Ki = KI;
	this->Kd = KD;
	this->midTemps[2] = {0};
	this->setPoint = 0;
	this->PWM1 = 0;
	this->PWM2 = 0;
	UpdateTemperature();
	this->LeftTempPID.setBangBang(BANGBANG_TEMP);
	this->LeftTempPID.setTimeStep(PID_TIME_STEP);
	this->RightTempPID.setBangBang(BANGBANG_TEMP);
	this->RightTempPID.setTimeStep(PID_TIME_STEP);
}

HotEnd::HotEnd(uint8_t * thermistorPins, uint8_t * PWMPins)
{
	HotEnd();
	this->ThermistorPins[0] = thermistorPins[0];
	this->ThermistorPins[1] = thermistorPins[1];
	this->PWMPins[0] = PWMPins[0];
	this->PWMPins[1] = PWMPins[1];
	InitIO();
}

void HotEnd::InitIO()
{
	pinMode(this->PWMPins[0], OUTPUT);
	pinMode(this->PWMPins[1], OUTPUT);
	pinMode(this->ledPin, OUTPUT);
}

void HotEnd::SetDesiredTemp(uint16_t & temp)
{
	this->setPoint = temp;
}

void HotEnd::Execute()
{
	static uint32_t delayTime1 = millis();
	static uint32_t delayTime2 = millis();
	if (millis() > delayTime1 + 200) 
	{
		QuickUpdateTemp();
		LeftTempPID.run();
		analogWrite(PWMPins[0], PWM1);
		digitalWrite(this->ledPin, LeftTempPID.atSetPoint(2));
		delayTime1 = millis();
	}

	if (millis() > delayTime2 + 300)
	{
		QuickUpdateTemp();
		RightTempPID.run();
		analogWrite(PWMPins[1], PWM2);
		digitalWrite(this->ledPin, RightTempPID.atSetPoint(4));
		delayTime2 = millis();
	}
}

bool HotEnd::UpdateTemperature()
{
	static uint32_t lastUpdate = millis();
	if (millis() > DELAY_UPDATE_TIME + lastUpdate)
	{
		this->midTemps[0] = MidLeft.analog2temp();
		this->midTemps[1] = MidRight.analog2temp();
		lastUpdate = millis();
		return true;
	}
	else
	{
		return false;
	}
}

void HotEnd::QuickUpdateTemp()
{
	this->midTemps[0] = MidLeft.analog2temp();
	this->midTemps[1] = MidRight.analog2temp();
}

