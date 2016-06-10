#pragma once
#include <cstdint>

class CHudCountdown : public CHudBase
{
	int8_t seconds_left;
	char name1[32];
	char name2[32];

public:
	virtual int Init() override;
	virtual int VidInit() override;
	virtual int Draw(float time) override;

	int MsgFunc_Countdown(const char* name, int size, void* buf);
};
