#pragma once

class CHudTimeout : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);
	int MsgFunc_Timeout(const char* name, int size, void* buf);

private:
	enum enumState { Inactive = 0, Called = 1, Pause = 2, Countdown = 3 };
	int       m_State;
	int       m_iTime;
};
#pragma once
