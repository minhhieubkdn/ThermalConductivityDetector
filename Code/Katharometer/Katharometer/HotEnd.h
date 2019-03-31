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

class HotEnd {
public:
	HotEnd();
	HotEnd(uint8_t * thermistorPins, uint8_t * PWMPins);
	void SetDesiredTemp(uint16_t &temp);
	void InitIO();
	bool UpdateTemperature();
	void Execute();

	AutoPID LeftTempPID = AutoPID(&midTemps[0], &setPoint, &PWM1, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
	AutoPID RightTempPID = AutoPID(&midTemps[1], &setPoint, &PWM2, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
	double Kp, Ki, Kd;
	uint8_t ThermistorPins[4] = {A0, A1, A2, A3};
	uint8_t PWMPins[2] = {10, 11};
	uint8_t ledPin = TEMP_LED_PIN;
	double midTemps[2];
private:
	void QuickUpdateTemp();
	thermistor MidLeft = thermistor(ThermistorPins[0], 0);
	thermistor MidRight = thermistor(ThermistorPins[1], 0);
	double setPoint;
	double PWM1;
	double PWM2;
};

#endif

