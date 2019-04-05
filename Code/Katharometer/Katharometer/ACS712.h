
#ifndef CURRENT_SENSOR_PIN
#define CURRENT_SENSOR_PIN A6
#endif
/*
VPERAMP = 185 for Module 5A
= 100 for Module  20A
= 66 for Module 30A
*/

#define VPERAMP 66
#define ACSOFFSET 2500

class ACS712 {
private:
	
	int RawValue = 0; // Raw value
	float Voltage = 0;
	float Amps = 0;
public:
	void Run();
	float ReadCurrent();
	float ReadVoltage();
};

void ACS712::Run() {
	this->RawValue = analogRead(CURRENT_SENSOR_PIN);
	this->Voltage = (this->RawValue / 1024.0) * 5000;
	this->Amps = ((this->Voltage - ACSOFFSET) / VPERAMP);
}

float ACS712::ReadCurrent()
{
	Run();
	return float(this->Amps);
}

float ACS712::ReadVoltage()
{
	Run();
	return float(this->Voltage);
}