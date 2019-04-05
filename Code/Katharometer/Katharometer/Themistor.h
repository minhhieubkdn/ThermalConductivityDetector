// Themistor.h

#ifndef _THEMISTOR_h
#define _THEMISTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <thermistor.h>
class Thermistor {
public:
	Thermistor(uint8_t thermPin);
	uint16_t ReadTemp();
	float ReadTempFloat();

private:
	void Read();
	thermistor therm = thermistor(A0, 0);
	uint16_t temperature;
	float floatTemp;
};

#endif

