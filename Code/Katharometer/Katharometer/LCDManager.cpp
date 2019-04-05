// 
// 
// 

#include "LCDManager.h"

LCDManager::LCDManager()
{
	StableButton.Init(this->mButtonArray, 6);
}

//not recommended
void LCDManager::AddNewMenu(OriginMenu * menu)
{
	LCDMenu.AddMenu(menu);
}

void LCDManager::Init()
{
	LCDMenu.UpdateScreen();
}

void LCDManager::LCDExecute()
{
	ExecuteMenuButton();
	LCDMenu.ExecuteEffect();
}

void LCDManager::ExecuteMenuButton()
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

void LCDManager::UpdateTempOnLCD()
{
	static unsigned long lasttime = millis();
	if (millis() > 200 + lasttime)
	{
		lasttime = millis();
		LCDMenu.UpdateScreen();
	}
}

void LCDManager::SetCurrentMenu(OriginMenu * _menu)
{
	LCDMenu.SetCurrentMenu(_menu);
}

void LCDManager::ReverseLed()
{
	digitalWrite(13, digitalRead(13) ^ 1);
}

