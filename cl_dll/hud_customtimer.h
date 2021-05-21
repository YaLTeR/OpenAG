#ifndef CUSTOMTIMER_H
#define CUSTOMTIMER_H

#pragma once
#include <cstdint>

class CHudCustomTimer : public CHudBase
{
	float m_flSeconds;
	float m_flTurnoffTime;

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	int UserCmd_CustomTimer();
};

#endif //CUSTOMTIMER_H