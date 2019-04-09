// StableButton.h

#ifndef _STABLEBUTTON_h
#define _STABLEBUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define HEATING_LED 12
#define RESPOND_LED 13

struct Button
{
	uint8_t pin;
	void(*func)() = NULL;
};

class StableButtonClass
{
 public:	 
	 void Init(uint8_t* buttonArray, uint8_t number);
	 bool IsPressing(uint8_t pin); 
	 bool IsPressed(uint8_t pin);

 private:
	 uint8_t buttonNumber;
	 Button *buttonArray;
	 unsigned long* counterArray;
	 uint16_t* maxDelayArray;		// Times to receive a button signal 
	 bool* isUpArray;
	 void InitIO();

};

extern StableButtonClass StableButton;

#endif

