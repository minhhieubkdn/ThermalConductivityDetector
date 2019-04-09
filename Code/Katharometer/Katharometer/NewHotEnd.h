// NewHotEnd.h

#ifndef _NEWHOTEND_h
#define _NEWHOTEND_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "CustomTempPID.h"
class NewHotEnd {
public:
	NewHotEnd();
	void SetTemperature(uint16_t _temp);
	void ReadAndSetTempTo(float * _tempArr);
	void Execute();
private:
	CustomTempPIDClass LeftPID = CustomTempPIDClass(A0, 10);
	CustomTempPIDClass RightPID = CustomTempPIDClass(A1, 11);
};

#endif

