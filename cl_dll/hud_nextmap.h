#pragma once
#include <cstdint>

class CHudNextMap : public CHudBase
{
	float draw_until;
	char next_map[32];

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	int MsgFunc_Nextmap(const char* name, int size, void* buf);
};
