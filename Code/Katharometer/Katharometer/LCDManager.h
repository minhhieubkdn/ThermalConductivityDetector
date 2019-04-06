// LCDManager.h

#ifndef _LCDMANAGER_h
#define _LCDMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "LCDMenu.h"
#include "StableButton.h"
#include <LiquidCrystal_I2C.h>

#ifndef NEW_BUTTON_ARRAY
#define LEFT 2
#define RIGHT 3
#define UP 4
#define DOWN 5
#define RETURN 6
#define ENTER 7
#endif

class LCDManager {
public:
	LCDManager();
	void AddNewMenu(OriginMenu* menu);
	void Init();
	void ExecuteMenuButton();
	void UpdateTempOnLCD();
	void SetCurrentMenu(OriginMenu* _menu);
	static void ReverseLed();
	void LCDExecute();
	void (*StartChecking)(void);

	uint8_t mButtonArray[6] = { LEFT, RIGHT, UP, DOWN, RETURN, ENTER };
};

#endif

