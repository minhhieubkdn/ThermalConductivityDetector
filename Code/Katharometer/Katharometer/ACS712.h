
#ifndef CURRENT_SENSOR_PIN
#define CURRENT_SENSOR_PIN A6
#endif

/*
SENSITIVITY = 185 for Module 5A
= 100 for Module  20A
= 66 for Module 30A
*/

#define SENSITIVITY 66
#define OFFSET_VOLTAGE 2500

class ACS712 {
public:
	
	float RawValue = 0; // Raw value
	float VRMS;
	float Voltage = 0;
	float Current = 0;
	/*void Run();
	float GetCurrent();
	float GetVoltage();*/
	void Execute();
};

//void ACS712::Run() {
//	this->RawValue = analogRead(CURRENT_SENSOR_PIN);
//	this->Voltage = (this->RawValue / 1024.0) * 5000;
//	this->Current = ((this->Voltage - OFFSET_VOLTAGE) / SENSITIVITY);
//}
//
//float ACS712::GetCurrent()
//{
//	Run();
//	return float(this->Current);
//}
//
//float ACS712::GetVoltage()
//{
//	Run();
//	return float(this->RawValue * 11.71875);
//}

void ACS712::Execute()
{
	int readValue;             //value read from the sensor
	static int maxValue = 0;          // store max value here
	static int minValue = 1024;          // store min value here

	static uint32_t start_time = millis();
	if ((millis() - start_time) > 1000) 
	{
		RawValue = ((maxValue - minValue) * 5.0) / 1024.0;
		VRMS = RawValue * 0.3535;
		Voltage = VRMS;
		Current = VRMS / 1000 * SENSITIVITY; 
		readValue = 0;
		maxValue = 0;
		minValue = 1024;
		start_time = millis();
	}
	else 
	{
		readValue = analogRead(CURRENT_SENSOR_PIN);
		if (readValue > maxValue)
		{
			maxValue = readValue;
		}
		if (readValue < minValue)
		{
			minValue = readValue;
		}
	}
}
