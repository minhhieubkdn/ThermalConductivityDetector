
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
private:
	
	int RawValue = 0; // Raw value
	float Voltage = 0;
	float Current = 0;
public:
	void Run();
	float ReadCurrent();
	float ReadVoltage();
};

void ACS712::Run() {
	pinMode(CURRENT_SENSOR_PIN, INPUT);
	this->RawValue = analogRead(CURRENT_SENSOR_PIN);
	this->Voltage = (this->RawValue / 1024.0) * 5000;
	this->Current = ((this->Voltage - OFFSET_VOLTAGE) / SENSITIVITY);
}

float ACS712::ReadCurrent()
{
	Run();
	return float(this->Current);
}

float ACS712::ReadVoltage()
{
	Run();
	return float(this->RawValue * 11.71875);
}
