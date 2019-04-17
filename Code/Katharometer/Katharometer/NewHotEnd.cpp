// 
// 
// 

#include "NewHotEnd.h"

NewHotEnd::NewHotEnd()
{
	LeftPID.init();
	RightPID.init();
}

void NewHotEnd::SetTemperature(uint16_t _temp)
{
	LeftPID.SetDesiredTemp(float(_temp));
	RightPID.SetDesiredTemp(float(_temp));
}

void NewHotEnd::ReadAndSetTempTo(float * _tempArr)
{
	_tempArr[0] = LeftPID.ReadTemp();
	_tempArr[1] = RightPID.ReadTemp();
}

void NewHotEnd::Execute()
{
	LeftPID.Run();
	RightPID.Run();
}
