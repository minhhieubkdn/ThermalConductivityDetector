// 
// 
// 

#include "HotEnd.h"

HotEnd::HotEnd()
{
	this->midLeftTemp = 0;
	this->midRightTemp = 0;
	this->setPoint = 0;
	this->PWM1 = 0;
	this->PWM2 = 0;
	UpdateTemperature();
	this->LeftTempPID.setBangBang(BANGBANG_TEMP);
	this->LeftTempPID.setTimeStep(PID_TIME_STEP);
	this->RightTempPID.setBangBang(BANGBANG_TEMP);
	this->RightTempPID.setTimeStep(PID_TIME_STEP);
}

HotEnd::HotEnd(uint8_t * _PWMPins)
{
	HotEnd();
	this->PWMPins[0] = _PWMPins[0];
	this->PWMPins[1] = _PWMPins[1];
	InitIO();
}

void HotEnd::InitIO()
{
	pinMode(this->PWMPins[0], OUTPUT);
	pinMode(this->PWMPins[1], OUTPUT);
	pinMode(TEMP_LED_PIN, OUTPUT);
}

void HotEnd::SetDesiredTemp(uint16_t & temp)
{
	this->setPoint = temp;
}

void HotEnd::Execute()
{
	//static uint32_t delayTime1 = millis();
	//static uint32_t delayTime2 = millis();

	//if (millis() > delayTime1 + 200) 
	//{
		QuickUpdateTemp();
		LeftTempPID.run();
		analogWrite(PWMPins[0], 255 - PWM1);
		digitalWrite(TEMP_LED_PIN, LeftTempPID.atSetPoint(4));
	//	delayTime1 = millis();
	//}

	//if (millis() > delayTime2 + 300)
	//{
		RightTempPID.run();
		analogWrite(PWMPins[1], 255 - PWM2);
		digitalWrite(TEMP_LED_PIN, RightTempPID.atSetPoint(4));
	//	delayTime2 = millis();
	//}
}

bool HotEnd::UpdateTemperature()
{
	static uint32_t lastUpdate = millis();
	if (millis() > DELAY_UPDATE_TIME + lastUpdate)
	{
		this->midLeftTemp = this->MidLeft.ReadTempFloat();
		this->midRightTemp = this->MidRight.ReadTempFloat();
		lastUpdate = millis();
		return true;
	}
	else
	{
		return false;
	}
}

void HotEnd::ReadAndSetMidTempsTo(float * _midTemps)
{
	this->QuickUpdateTemp();
	_midTemps[0] = this->midLeftTemp;
	_midTemps[1] = this->midRightTemp;
}

void HotEnd::QuickUpdateTemp()
{
	this->midLeftTemp = MidLeft.ReadTempFloat();
	this->midRightTemp = MidRight.ReadTempFloat();
}

