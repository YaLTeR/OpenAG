#pragma once
#include <cstdint>

class CHudJumpspeed : public CHudBase
{
	uint16_t speed;

	cvar_t* hud_jumpspeed;
	cvar_t* hud_jumpspeed_below_cross;
	cvar_t* hud_jumpspeed_height;

	int fadingFrom[3];
	float prevVel[3] = { 0.0f, 0.0f, 0.0f };
	float lastTime;
	double passedTime;

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	void UpdateSpeed(const float velocity[3]);
};
