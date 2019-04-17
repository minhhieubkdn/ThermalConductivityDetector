// 
// 
// 

#include "CustomTempPID.h"

CustomTempPIDClass::CustomTempPIDClass(uint8_t _thermistorPin, uint8_t _PWM_pin)
{
	this->ThermistorPin = _thermistorPin;
	this->therm = Thermistor(_thermistorPin);
	this->PWM_pin = _PWM_pin;
	init();
}

void CustomTempPIDClass::init()
{
	pinMode(PWM_pin, OUTPUT);
}

void CustomTempPIDClass::Run()
{
	temperature_read = ReadTemp();
	if (set_temperature == 0 || (temperature_read > set_temperature) && (temperature_read - set_temperature > 2)) {
		analogWrite(this->PWM_pin, 255);
		Time = millis();
		return;
	}

	PID_error = set_temperature - temperature_read;
	PID_p = 0.01 * kp * PID_error;
	PID_i = 0.01 * PID_i + (ki * PID_error);

	timePrev = Time;                           
	Time = millis();                            
	elapsedTime = (Time - timePrev) / 1000;
	PID_d = 0.01*kd*((PID_error - previous_error) / elapsedTime);
	PID_value = PID_p + PID_i + PID_d;

	if (PID_value < 0)
	{
		PID_value = 0;
	}
	if (PID_value > 255)
	{
		PID_value = 255;
	}
	analogWrite(this->PWM_pin, 255 - this->PID_value);
	previous_error = PID_error;
}

uint16_t CustomTempPIDClass::ReadTemp()
{
	float t = this->therm.ReadTempFloat();
	uint16_t _temp = roundf(t);
	return _temp;
}

void CustomTempPIDClass::SetDesiredTemp(uint16_t _temp)
{
	this->set_temperature = _temp;
}


