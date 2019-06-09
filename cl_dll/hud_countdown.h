#pragma once
#include <cstdint>

class CHudCountdown : public CHudBase
{
	float draw_until;

	int8_t seconds_left;
	char name1[32];
	char name2[32];

	cvar_t* cl_autorecord;

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	int MsgFunc_Countdown(const char* name, int size, void* buf);
};
