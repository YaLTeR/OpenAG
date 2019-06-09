#pragma once
#include <cstdint>

class CHudPlayerId : public CHudBase
{
	float draw_until;

	int player_id;
	bool teammate;
	int health;
	int armor;

	cvar_t* hud_playerid;

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	int MsgFunc_PlayerId(const char* name, int size, void* buf);
};
