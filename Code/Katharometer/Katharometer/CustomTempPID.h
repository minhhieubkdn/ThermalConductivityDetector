// CustomTempPID.h


#ifndef _CUSTOMTEMPPID_h
#define _CUSTOMTEMPPID_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Themistor.h"

class CustomTempPIDClass
{
 protected:
	 //int kp = 90;   int ki = 30;   int kd = 80;
	 int kp = 90;   int ki = 30;   int kd = 80;
	 int PID_p = 0;    
	 int PID_i = 0;    
	 int PID_d = 0;
	 float set_temperature = 0; 
	 float temperature_read = 0;
	 float PID_error = 0;
	 float previous_error = 0;
	 float elapsedTime, Time, timePrev;
	 float PID_value = 0;
	 uint8_t ThermistorPin = A0;
	 uint8_t PWM_pin;
	 Thermistor therm = Thermistor(ThermistorPin);
 public:
	 CustomTempPIDClass(uint8_t _thermistorPin, uint8_t _PWM_pin);
	void init();
	void Run();
	uint16_t ReadTemp();
	void SetDesiredTemp(uint16_t _temp);
};

#endif

