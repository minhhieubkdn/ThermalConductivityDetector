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
uint16_t MidLeftTemp;
uint16_t MidRightTemp;
uint16_t OutLeftTemp;
uint16_t OutRightTemp;

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
float lamda;

OriginMenu* firstMenu = new OriginMenu();
Label* lbDesiredTemp;
VariableText* vtDesiredTemp;
SubMenu* MeasureSubmenu;
	Label* lbMyHeart;
	SubMenu* smYes;
	Label* smNo;

OriginMenu* sttMenu = new OriginMenu();
Label* lbMidTemps;
VariableText* vtMidleftTemp;
VariableText* vtMidRightTemp;
Label* lbOutTemp;
VariableText* vtOutLeftTemp;
VariableText* vtOutRightTemp;

OriginMenu* ACSMenu = new OriginMenu();
Label* lbCurrent;
VariableText* vtCurrent;
Label* lbVoltage;
VariableText* vtVoltage;

void setup() {
	LCDMenu.Init(lcd, "Katharometer");
	InitLCDMenu();
	StableButton.Init(manager.mButtonArray, 6);
}

void loop() 
{
	static uint32_t last = millis();
	ExecuteMenuButton();
	LCDMenu.ExecuteEffect();
	if (millis() > last + 800)
	{
		last = millis();
		Current = acs.ReadCurrent();
		Voltage = acs.ReadVoltage();
		readTemp();
		LCDMenu.UpdateScreen();
	}
}

void readTemp()
{
	Heater.ReadAndSetMidTempsTo(CurrentMidTempsFloat);
	MidLeftTemp = CurrentMidTempsFloat[0];
	MidRightTemp = CurrentMidTempsFloat[1];
	OutLeftTemp = OutLeft.ReadTemp();
	OutRightTemp = OutRight.ReadTemp();
}

void PrintTempToSerial() {
	MidLeftTemp = CurrentMidTempsFloat[0];
	MidRightTemp = CurrentMidTempsFloat[1];
	Serial.println(MidLeftTemp);
	Serial.println(MidRightTemp);
	Serial.println(OutLeftTemp);
	Serial.println(OutRightTemp);
	Serial.println();
}

void InitLCDMenu() 
{
	lbDesiredTemp = new Label(firstMenu, "DesiredTemp: ", 0, 0);
	vtDesiredTemp = new VariableText(firstMenu, 100, 13, 0);
	vtDesiredTemp->HandleWhenValueChange = []() 
	{
		DesiredMidTemp = roundf(vtDesiredTemp->GetValue());
		Heater.SetDesiredTemp(DesiredMidTemp);
	};
	MeasureSubmenu = new SubMenu(firstMenu, "Start measure", 0, 1);
	lbMyHeart = new Label(MeasureSubmenu->Container, "Start heating?", 0, 0);
	{
		smYes = new SubMenu(MeasureSubmenu->Container, "Yes", 0, 1);
		smNo = new Label(MeasureSubmenu->Container, "No", 7, 1);
	}

	lbMidTemps = new Label(sttMenu, "MidTem: ", 0, 0);
	vtMidleftTemp = new VariableText(sttMenu, MidLeftTemp, 8, 0);
	vtMidleftTemp->SetExternalValue(&MidLeftTemp);
	vtMidRightTemp = new VariableText(sttMenu, MidRightTemp, 12, 0);
	vtMidRightTemp->SetExternalValue(&MidRightTemp);

	lbOutTemp = new Label(sttMenu, "OutTem: ", 0, 1);
	vtOutLeftTemp = new VariableText(sttMenu, OutLeftTemp, 8, 1);
		vtOutLeftTemp->SetExternalValue(&OutLeftTemp);
	vtOutRightTemp = new VariableText(sttMenu, OutRightTemp, 12, 1);
		vtOutRightTemp->SetExternalValue(&OutRightTemp);
		
	lbCurrent = new Label(ACSMenu, "Current : ", 0, 0);
	vtCurrent = new VariableText(ACSMenu, Current, 10, 0);
	lbVoltage = new Label(ACSMenu, "Voltage : ", 0, 1);
	vtVoltage = new VariableText(ACSMenu, Voltage, 10, 1);
	
	LCDMenu.AddMenu(firstMenu);
	LCDMenu.AddMenu(sttMenu);
	LCDMenu.AddMenu(ACSMenu);
	LCDMenu.SetCurrentMenu(firstMenu);
	LCDMenu.UpdateScreen();
}

float ComputeLamda() 
{
	float U = acs.ReadVoltage();
	float I = acs.ReadCurrent();
	float lamda1, lamda2;
	Heater.ReadAndSetMidTempsTo(CurrentMidTempsFloat);
	lamda1 = U * I * 0.02 / (0.09*(CurrentMidTempsFloat[0] - OutLeft.ReadTempFloat()));
	lamda2 = U * I * 0.02 / (0.09*(CurrentMidTempsFloat[1] - OutRight.ReadTempFloat()));
	return (lamda1 + lamda2) / 2;
}

void ExecuteMenuButton()
{
	if (StableButton.IsPressed(LEFT))
	{
		LCDMenu.MoveCursorLeft();
	}

	if (StableButton.IsPressed(RIGHT))
	{
		LCDMenu.MoveCursorRight();
	}

	if (StableButton.IsPressed(UP))
	{
		LCDMenu.MoveCursorUp();
	}

	if (StableButton.IsPressed(DOWN))
	{
		LCDMenu.MoveCursorDown();
	}

	if (StableButton.IsPressed(RETURN))
	{
		LCDMenu.Return();
	}

	if (StableButton.IsPressed(ENTER))
	{
		LCDMenu.Enter();
	}
}