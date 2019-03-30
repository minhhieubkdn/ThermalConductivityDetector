// HotEnd.h

#ifndef _HOTEND_h
#define _HOTEND_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <AutoPID.h>
#include <thermistor.h>

#ifndef OUTPUT_MIN
#define OUTPUT_MIN 0
#endif

#ifndef OUTPUT_MAX
#define OUTPUT_MAX 255
#endif

#ifndef KP
#define KP 0.12
#endif

#ifndef KI
#define KI 0.0003
#endif

#ifndef KD
#define KD 0
#endif

class HotEnd {
public:
	HotEnd();
	HotEnd(uint8_t &thermistorPins, uint8_t &PWMPins);
	void SetDesiredTemp(int &temp);

	AutoPID TempPID;
	thermistor Themistor[4];
	double Kp, Ki, Kd;
	uint8_t ThermistorPins[4];
	uint8_t PWMPins[2];
private:
	double temperature;
	double setPoint;
	double PWMValue;
};

#endif

