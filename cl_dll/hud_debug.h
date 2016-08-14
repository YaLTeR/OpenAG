#pragma once

class CHudDebug : public CHudBase
{
	cvar_t* hud_debug;
	
public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);
};
