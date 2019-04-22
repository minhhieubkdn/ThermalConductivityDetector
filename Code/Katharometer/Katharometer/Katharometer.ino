/*
Name:		Katharometer.ino
Created:	3/30/2019 3:20:17 PM
Author:	Minh Hieu
*/

#include "StableTemperature.h"
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

#define MODE_ADDRESS 0
#define LAST_TEMP_ADDRESS 1
#define LAST_PWM_ADDRESS 2

#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "NewHotEnd.h"
#include "CustomTempPID.h"
#include "Themistor.h"
#include "LCDMenu.h"
#include "StableButton.h"

uint8_t mButtonArray[6] = { LEFT, RIGHT, UP, DOWN, RETURN, ENTER };
LiquidCrystal_I2C *lcd = new LiquidCrystal_I2C(0x27, 16, 2);

uint16_t DesiredMidTemp = 0;
uint16_t MidLeftTemp;
uint16_t MidRightTemp;
uint16_t OutLeftTemp;
uint16_t OutRightTemp;

NewHotEnd Heater = NewHotEnd();
Thermistor OutLeft = Thermistor(A2);
Thermistor OutRight = Thermistor(A3);

float CurrentMidTempsFloat[2];

OriginMenu* firstMenu = new OriginMenu();
Label* lbDesiredTemp;
VariableText* vtDesiredTemp;
SubMenu* MeasureSubmenu;
	Label* lbStartHeating;
	FunctionText* ftYes;
	FunctionText* ftNo;
FunctionText* ftMode;

OriginMenu* sttMenu = new OriginMenu();
Label* lbLeftTemp;
Label* lbMidLeftTemp;
Label* lbOutLeftTemp;
Label* lbLeftDeltaTemp;
Label* lbRightTemp;
Label* lbMidRightTemp;
Label* lbOutRightTemp;
Label* lbRightDeltaTemp;

//OriginMenu* ACSMenu = new OriginMenu();
//Label* lbCurrent;
//Label* vtCurrent;
//Label* lbVoltage;
//Label* vtVoltage;

//OriginMenu* Caculator = new OriginMenu();
//Label* lbLamda1, *lbLamda2;
//Label* lbLamdaValue1, *lbLamdaValue2;

bool isHeating = false;
bool mode = true; // == PID mode
byte PWM = 0;

uint16_t lastDeltaT = 0;
uint16_t StableDeltaT = 0;

void setup()
{
	LCDMenu.Init(lcd, "Katharometer");
	InitLCDMenu();
	InitData();
	StableButton.Init(mButtonArray, 6);
}

void loop()
{
	static uint32_t last = millis();
	ExecuteMenuButton();
	LCDMenu.ExecuteEffect();
	if (mode)
	{
		Heater.Execute();
	}
	else
	{
		analogWrite(10, 255 - PWM);
		analogWrite(11, 255 - PWM);
	}
	if (millis() > last + 800)
	{
		last = millis();
		readTemp();
		UpdateLabel();
		LCDMenu.UpdateScreen();
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

void InitLCDMenu()
{
	//firstMenu
	{
		lbDesiredTemp = new Label(firstMenu, "DesiredTemp: ", 0, 0);
		vtDesiredTemp = new VariableText(firstMenu, 100, 13, 0);
		MeasureSubmenu = new SubMenu(firstMenu, "Heat", 0, 1);
		{
			lbStartHeating = new Label(MeasureSubmenu->Container, "Start heating?", 0, 0);
			ftYes = new FunctionText(MeasureSubmenu->Container, "Yes", 0, 1);
			ftNo = new FunctionText(MeasureSubmenu->Container, "No", 7, 1);
		}
		ftMode = new FunctionText(firstMenu, "PID", 8, 1);
	}

	//sttMenu
	{
		lbLeftTemp = new Label(sttMenu, "Left:", 0, 0);
		lbMidLeftTemp = new Label(sttMenu, "", 6, 0);
		lbOutLeftTemp = new Label(sttMenu, "", 10, 0);
		lbLeftDeltaTemp = new Label(sttMenu, "", 14, 0);

		lbRightTemp = new Label(sttMenu, "Right:", 0, 1);
		lbMidRightTemp = new Label(sttMenu, "", 6, 1);
		lbOutRightTemp = new Label(sttMenu, "", 10, 1);
		lbRightDeltaTemp = new Label(sttMenu, "", 14, 1);
	}

	LCDMenu.AddMenu(firstMenu);
	LCDMenu.AddMenu(sttMenu);
	LCDMenu.SetCurrentMenu(firstMenu);
	LCDMenu.UpdateScreen();
}

void InitData()
{
	DesiredMidTemp = 0;
	vtDesiredTemp->HandleWhenValueChange = ChangeDesiredTemp;
	ftYes->Function = StartHeating;
	ftNo->Function = CancelHeating;
	ftMode->Function = ChangeMode;

	EEPROM.begin();
	mode = EEPROM.read(MODE_ADDRESS);
	if (mode)
	{
		lbDesiredTemp->SetText("DesireTem:");
		ftMode->SetText("PID_MODE");
		vtDesiredTemp->SetValue(EEPROM.read(LAST_TEMP_ADDRESS));
		vtDesiredTemp->Resolution = 5;
	}
	else
	{
		vtDesiredTemp->SetValue(EEPROM.read(LAST_PWM_ADDRESS));
		vtDesiredTemp->Resolution = 1;
		lbDesiredTemp->SetText("PWM Value:");
		ftMode->SetText("PWM_MODE");
	}
	vtDesiredTemp->IsTextChanged = true;
}

void StartHeating(DisplayElement* ft)
{
	if (mode)
	{
		if (!isHeating)
		{
			if (DesiredMidTemp == 0)
			{
				ChangeDesiredTemp();
			}
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
	else
	{
		if (!isHeating) 
		{
			if (PWM == 0)
			{
				PWM = roundf(vtDesiredTemp->GetValue());
			}
			analogWrite(10, 255 - PWM);
			analogWrite(11, 255 - PWM);
			isHeating = true;
			LCDMenu.Return();
			LCDMenu.Return();
		}
		else
		{
			isHeating = false;
			PWM = 0;
			analogWrite(10, 255);
			analogWrite(11, 255);
			LCDMenu.Return();
		}
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
	lbLeftDeltaTemp->SetText(String(MidLeftTemp - OutLeftTemp));
	lbRightDeltaTemp->SetText(String(MidRightTemp - OutRightTemp));
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

void ChangeDesiredTemp() 
{
	if (mode)
	{
		DesiredMidTemp = roundf(vtDesiredTemp->GetValue());
		EEPROM.update(LAST_TEMP_ADDRESS, DesiredMidTemp);
	}
	else
	{
		byte _pwm = roundf(vtDesiredTemp->GetValue());
		if (isHeating)
		{
			PWM = _pwm;
		}
		EEPROM.update(LAST_PWM_ADDRESS, _pwm);
	}
}

void ChangeMode(DisplayElement* de)
{
	mode = !mode;
	if (mode)
	{
		lbDesiredTemp->SetText("DesireTem:");
		vtDesiredTemp->SetValue(EEPROM.read(LAST_TEMP_ADDRESS));
		vtDesiredTemp->Resolution = 5;
		ftMode->SetText("PID_MODE");
		ftMode->IsTextChanged = true;
	}
	else
	{
		lbDesiredTemp->SetText("PWM Value:");
		vtDesiredTemp->SetValue(EEPROM.read(LAST_PWM_ADDRESS));
		vtDesiredTemp->Resolution = 1;
		ftMode->SetText("PWM_MODE");
		ftMode->IsTextChanged = true;
	}
	EEPROM.update(MODE_ADDRESS, mode);
}