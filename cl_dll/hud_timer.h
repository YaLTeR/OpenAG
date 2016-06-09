#pragma once

class CHudTimer : public CHudBase
{
	int seconds_total;
	int seconds_passed;

public:
	virtual int Init() override;
	virtual int VidInit() override;
	virtual int Draw(float time) override;

	int MsgFunc_Timer(const char* name, int size, void* buf);

	cvar_t* hud_timer;
};
