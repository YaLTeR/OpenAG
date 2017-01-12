#pragma once
#include <cstdint>

class CHudCrosshairs : public CHudBase
{
public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);
};
