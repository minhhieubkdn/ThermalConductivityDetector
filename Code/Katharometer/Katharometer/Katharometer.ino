/*
 Name:		Katharometer.ino
 Created:	3/30/2019 3:20:17 PM
 Author:	Minh Hieu
*/

#define NEW_BUTTON_ARRAY
#define RETURN 7
#define LEFT 2
#define RIGHT 3
#define DOWN 5
#define UP 4
#define ENTER 6

#define HEATING_LED 12
#define RESPOND_LED 13

#include <LiquidCrystal_I2C.h>
#include "LCDManager.h"
#include "Themistor.h"
#include "HotEnd.h"
#include "LCDMenu.h"
#include "ACS712.h"

struct IntTemp
{
	uint32_t leftTemp;
	uint32_t righttemp;
};

struct FloatTemp
{
	float leftTemp;
	float rightTemp;
};

uint8_t outThermistorPins[] = { A2, A3 };
uint8_t PWMPins[] = { 10, 11 };
LCDManager manager = LCDManager();
LiquidCrystal_I2C *lcd = new LiquidCrystal_I2C(0x27, 16, 2);

uint16_t DesiredMidTemp;
IntTemp Mid;
IntTemp Out;

float computeDeltaTemp(uint32_t t1, uint32_t t2)
{
	return float(t1 - t2);
}

ACS712 acs;
HotEnd Heater = HotEnd(PWMPins);
Thermistor OutLeft = Thermistor(outThermistorPins[0]);
Thermistor OutRight = Thermistor(outThermistorPins[1]);

float CurrentMidTempsFloat[2];
float Current;
float Voltage;

void setup() {
	DesiredMidTemp = 100;
	Serial.begin(9600);
	Heater.SetDesiredTemp(DesiredMidTemp);
	manager.Init();
	InitLCDMenu();
}

void loop() 
{
	static unsigned long last = millis();
	Heater.Execute();
	manager.LCDExecute();
	LCDMenu.ExecuteEffect();
	LCDMenu.UpdateScreen();
	if (millis() > last + 500)
	{
		last = millis();
		readTemp();
		PrintTempToSerial();
		Current = acs.ReadCurrent();
		Voltage = acs.ReadVoltage();
	}
}

void readTemp()
{
	Heater.ReadAndSetMidTempsTo(CurrentMidTempsFloat);
	Out.leftTemp = OutLeft.ReadTemp();
	Out.righttemp = OutRight.ReadTemp();
}

void PrintTempToSerial() {
	Mid.leftTemp = CurrentMidTempsFloat[0];
	Mid.righttemp = CurrentMidTempsFloat[1];
	Serial.println(Mid.leftTemp);
	Serial.println(Mid.righttemp);
	Serial.println(Out.leftTemp);
	Serial.println(Out.righttemp);
	Serial.println();
}

void InitLCDMenu() 
{
	OriginMenu* firstMenu = new OriginMenu();
	{
		Label* lbDesiredTemp = new Label(firstMenu, "DesiredTemp: ", 0, 0);
		VariableText* vtDesiredTemp = new VariableText(firstMenu, float(DesiredMidTemp), 10, 0);
		SubMenu* MeasureSubmenu = new SubMenu(firstMenu, "Start measure", 0, 1);
		{
			Label* lbMyHeart = new Label(MeasureSubmenu->Container, "Start heating?", 0, 0);
			Label* ftYes = new Label(MeasureSubmenu->Container, "Yes", 2, 1);
			Label* ftNo = new Label(MeasureSubmenu->Container, "No", 10, 1);
		}
	}

	OriginMenu* sttMenu = new OriginMenu();
	{
		Label* lbMidTemps = new Label(sttMenu, "MidTem: ", 0, 0);
		VariableText* vtMidleftTemp = new VariableText(sttMenu, float(Mid.leftTemp), 8, 0);
		VariableText* vtMidRightTemp = new VariableText(sttMenu, Mid.righttemp, 12, 0);
		Label* lbOutTemp = new Label(sttMenu, "OutTem: ", 0, 1);
		VariableText* vtOutLeftTemp = new VariableText(sttMenu, Out.leftTemp, 8, 1);
		VariableText* vtOutRightTemp = new VariableText(sttMenu, Out.righttemp, 12, 1);
	}

	OriginMenu* ACSMenu = new OriginMenu();
	{
		Label* lbCurrent = new Label(ACSMenu, "Current : ", 0, 0);
		VariableText* vtCurrent = new VariableText(ACSMenu, Current, 10, 0);
		Label* lbVoltage = new Label(ACSMenu, "Voltage : ", 0, 1);
		VariableText* vtVoltage = new VariableText(ACSMenu, Voltage, 10, 1);
	}

	OriginMenu* resultMenu = new OriginMenu();
	{
		Label* lbDeltaT = new Label(resultMenu, "dT1:", 0, 0);
		VariableText* vtDeltaTemp1 = new VariableText(resultMenu, computeDeltaTemp(Mid.leftTemp, Out.leftTemp), 4, 0);
		Label * lbCurrent = new Label(resultMenu, "dT2:", 8, 0);
		VariableText* vtDeltaTemp2 = new VariableText(resultMenu, computeDeltaTemp(Mid.righttemp, Out.righttemp), 12, 0);
	}

	OriginMenu * lamdaMenu = new OriginMenu();
	{
		Label* lbLamda = new Label(lamdaMenu, ":", 0, 0);
		VariableText* vtLamda = new VariableText(lamdaMenu, ComputeLamda(), 8, 0);
	}

	LCDMenu.Init(lcd, "Katharometer");
	LCDMenu.AddMenu(firstMenu);
	LCDMenu.AddMenu(sttMenu);
	LCDMenu.AddMenu(ACSMenu);
	LCDMenu.AddMenu(resultMenu);
	LCDMenu.AddMenu(lamdaMenu);
	LCDMenu.SetCurrentMenu(firstMenu);
	LCDMenu.UpdateScreen();
}

float ComputeLamda() 
{
	float U = acs.ReadVoltage();
	float I = acs.ReadCurrent();
	float lamda1, lamda2;
	Heater.ReadAndSetMidTempsTo(CurrentMidTempsFloat);
	lamda1 = U * I*0.02 / (0.09*(CurrentMidTempsFloat[0] - OutLeft.ReadTempFloat()));
	lamda2 = U * I*0.02 / (0.09*(CurrentMidTempsFloat[1] - OutRight.ReadTempFloat()));
	return (lamda1 + lamda2) / 2;
}