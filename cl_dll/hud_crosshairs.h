#pragma once
#include <cstdint>

class CHudCrosshairs : public CHudBase
{
	cvar_t* hud_crosshairs_test;

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);
};
