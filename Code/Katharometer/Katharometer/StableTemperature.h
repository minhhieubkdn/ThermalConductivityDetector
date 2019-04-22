// StableTemperature.h

#ifndef _STABLETEMPERATURE_h
#define _STABLETEMPERATURE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class StableDeltaT {
public:
	uint16_t deltaT;
	unsigned long time;
	bool isStable;

};


#endif

