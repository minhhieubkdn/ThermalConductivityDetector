// HotEnd.h

#ifndef _HOTEND_h
#define _HOTEND_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AutoPID.h"
#include "Themistor.h"

#ifndef MID_LEFT_THERMISTOR
#define MID_LEFT_THERMISTOR A0
#endif

#ifndef MID_RIGHT_THERMISTOR
#define MID_RIGHT_THERMISTOR A1
#endif

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

#ifndef DELAY_UPDATE_TIME 
#define DELAY_UPDATE_TIME 1000
#endif

#ifndef BANGBANG_TEMP
#define BANGBANG_TEMP 4
#endif

#ifndef PID_TIME_STEP
#define PID_TIME_STEP 4000
#endif

#ifndef TEMP_LED_PIN
#define TEMP_LED_PIN 12
#endif

#ifndef PWM_PIN_1
#define PWM_PIN_1 10
#endif

#ifndef PWM_PIN_2
#define PWM_PIN_2 11
#endif

class HotEnd {
public:
	HotEnd();
	HotEnd(uint8_t * _PWMPins);
	void SetDesiredTemp(uint16_t &temp);
	void InitIO();
	bool UpdateTemperature();
	void ReadAndSetMidTempsTo(float *_midTemps);
	void Execute();

	AutoPID LeftTempPID = AutoPID(&midTemps[0], &setPoint, &PWM1, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
	AutoPID RightTempPID = AutoPID(&midTemps[1], &setPoint, &PWM2, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
	uint8_t PWMPins[2] = { PWM_PIN_1, PWM_PIN_2 };
	float midTemps[2];
	Thermistor MidLeft = Thermistor(MID_LEFT_THERMISTOR);
	Thermistor MidRight = Thermistor(MID_RIGHT_THERMISTOR);
private:
	void QuickUpdateTemp();
	float setPoint;
	float PWM1;
	float PWM2;
};

#endif

