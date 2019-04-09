/*
 Name:		Katharometer.ino
 Created:	3/30/2019 3:20:17 PM
 Author:	Minh Hieu
*/

#include "NewHotEnd.h"
#include "CustomTempPID.h"
#define NEW_BUTTON_ARRAY
#define RETURN 7
#define LEFT 2
#define RIGHT 3
#define DOWN 5
#define UP 4
#define ENTER 6

#define KP 60
#define KI 30
#define KD 80

#define HEATING_LED 12
#define RESPOND_LED 13

#include <LiquidCrystal_I2C.h>
#include "LCDManager.h"
#include "Themistor.h"
#include "LCDMenu.h"
#include "ACS712.h"
#include "NewHotEnd.h"

struct FloatTemp
{
	float leftTemp;
	float rightTemp;
};

uint8_t outThermistorPins[] = { A2, A3 };
uint8_t PWMPins[] = { 10, 11 };
LCDManager manager = LCDManager();
LiquidCrystal_I2C *lcd = new LiquidCrystal_I2C(0x27, 16, 2);

uint16_t DesiredMidTemp = 0;
uint16_t MidLeftTemp;
uint16_t MidRightTemp;
uint16_t OutLeftTemp;
uint16_t OutRightTemp;

ACS712 acs;
NewHotEnd Heater = NewHotEnd();
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
	Label* lbStartHeating;
	FunctionText* smYes;
	FunctionText* smNo;

OriginMenu* sttMenu = new OriginMenu();
Label* lbMidTemps;
Label* lbMidLeftTemp;
Label* lbMidRightTemp;
Label* lbOutTemp;
Label* lbOutLeftTemp;
Label* lbOutRightTemp;

OriginMenu* ACSMenu = new OriginMenu();
Label* lbCurrent;
Label* vtCurrent;
Label* lbVoltage;
Label* vtVoltage;

bool isHeating = false;

void setup()
{
	LCDMenu.Init(lcd, "Katharometer");
	LCDMenu.TurnCursor(false);
	InitLCDMenu();
	InitData();
	Serial.begin(9600);
	StableButton.Init(manager.mButtonArray, 6);
}

void loop() 
{
	static uint32_t last = millis();
	ExecuteMenuButton();
	LCDMenu.ExecuteEffect();
	Heater.Execute();
	if (millis() > last + 800)
	{
		last = millis();
		Current = acs.ReadCurrent();
		Voltage = acs.ReadVoltage();
		readTemp();
		UpdateLabel();
		LCDMenu.UpdateScreen();
		PrintTempToSerial();
	}
}

void readTemp()
{
	Heater.ReadAndSetTempTo(CurrentMidTempsFloat);
	MidLeftTemp = CurrentMidTempsFloat[0];
	MidRightTemp = CurrentMidTempsFloat[1];
	OutLeftTemp = OutLeft.ReadTemp();
	OutRightTemp = OutRight.ReadTemp();
}

void PrintTempToSerial()
{
	MidLeftTemp = CurrentMidTempsFloat[0];
	MidRightTemp = CurrentMidTempsFloat[1];
	Serial.println(MidLeftTemp);
	Serial.println(MidRightTemp);
	Serial.println(OutLeftTemp);
	Serial.println(OutRightTemp);
	Serial.println(DesiredMidTemp);
	Serial.println();
}

void InitLCDMenu() 
{
	//firstMenu
	{
		lbDesiredTemp = new Label(firstMenu, "DesiredTemp: ", 0, 0);
		vtDesiredTemp = new VariableText(firstMenu, 100, 13, 0);
		MeasureSubmenu = new SubMenu(firstMenu, "Measure", 0, 1);
		{
			lbStartHeating = new Label(MeasureSubmenu->Container, "Start heating?", 0, 0);
			smYes = new FunctionText(MeasureSubmenu->Container, "Yes", 0, 1);
			smNo = new FunctionText(MeasureSubmenu->Container, "No", 7, 1);
		}
	}

	//sttMenu
	{
		lbMidTemps = new Label(sttMenu, "MidTem: ", 0, 0);
		lbMidLeftTemp = new Label(sttMenu, "", 8, 0);
		lbMidRightTemp = new Label(sttMenu, "", 12, 0);
		lbOutTemp = new Label(sttMenu, "OutTem: ", 0, 1);
		lbOutLeftTemp = new Label(sttMenu, "", 8, 1);
		lbOutRightTemp = new Label(sttMenu, "", 12, 1);
	}

	//ACSMenu
	{
		lbCurrent = new Label(ACSMenu, "Current : ", 0, 0);
		vtCurrent = new Label(ACSMenu, String(Current), 10, 0);
		lbVoltage = new Label(ACSMenu, "Voltage : ", 0, 1);
		vtVoltage = new Label(ACSMenu, String(Voltage), 10, 1);
	}
	
	LCDMenu.AddMenu(firstMenu);
	LCDMenu.AddMenu(sttMenu);
	LCDMenu.AddMenu(ACSMenu);
	LCDMenu.SetCurrentMenu(firstMenu);
	LCDMenu.UpdateScreen();
}

void InitData()
 {
	DesiredMidTemp = 0;
	vtDesiredTemp->HandleWhenValueChange = ChangeDesiredTemp;
	smYes->Function = StartHeating;
	smNo->Function = CancelHeating;
}

void StartHeating(DisplayElement* ft) {
	if (!isHeating)
	{
		isHeating = true;
		Heater.SetTemperature(DesiredMidTemp);
		LCDMenu.Return();
		LCDMenu.Return();
	}
	else
	{
		isHeating = false;
		DesiredMidTemp = 0;
		Heater.SetTemperature(DesiredMidTemp);
		LCDMenu.Return();
	}
}

void CancelHeating(DisplayElement* ft)
{
	LCDMenu.Return();
}

void InitIO()
{
	pinMode(HEATING_LED, OUTPUT);
	pinMode(RESPOND_LED, OUTPUT);
}

void UpdateLabel() 
{
	if (isHeating)
	{
		lbStartHeating->SetText("Cancel heating?");
	}
	else
	{
		lbStartHeating->SetText("Start heating?");
	}

	lbMidLeftTemp->SetText(String(MidLeftTemp));
	lbMidRightTemp->SetText(String(MidRightTemp));
	lbOutLeftTemp->SetText(String(OutLeftTemp));
	lbOutRightTemp->SetText(String(OutRightTemp));
	vtCurrent->SetText(String(Current));
	vtVoltage->SetText(String(Voltage));
}

float ComputeLamda() 
{
	float U = acs.ReadVoltage();
	float I = acs.ReadCurrent();
	float lamda1, lamda2;
	Heater.ReadAndSetTempTo(CurrentMidTempsFloat);
	lamda1 = U * I * 0.02 / (0.09 * (CurrentMidTempsFloat[0] - OutLeft.ReadTempFloat()));
	lamda2 = U * I * 0.02 / (0.09 * (CurrentMidTempsFloat[1] - OutRight.ReadTempFloat()));
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

void ChangeDesiredTemp() {
	DesiredMidTemp = roundf(vtDesiredTemp->GetValue());
}