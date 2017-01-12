#pragma once
#include <cstdint>

class CHudTimeout : public CHudBase
{
	enum class TimeoutStatus : uint8_t
	{
		HIDDEN = 0,

		STARTING = 1,
		PAUSING = 2,
		IN_PROGRESS = 3
	};

	TimeoutStatus status;
	uint8_t seconds;

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	int MsgFunc_Timeout(const char* name, int size, void* buf);
};
