#pragma once
#include <cstdint>

class CHudSuddenDeath : public CHudBase
{
public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	int MsgFunc_SuddenDeath(const char* name, int size, void* buf);
};
