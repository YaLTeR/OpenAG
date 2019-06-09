#pragma once
#include <cstdint>

class CHudSpeedometer : public CHudBase
{
	uint16_t speed;

	cvar_t* hud_speedometer;
	cvar_t* hud_speedometer_below_cross;

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	void UpdateSpeed(const float velocity[2]);
};
