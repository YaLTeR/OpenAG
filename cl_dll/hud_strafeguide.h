#pragma once
#include <complex>

class CHudStrafeGuide : public CHudBase
{
	double angles[6];
	
	char debug[256] = "";
	
	cvar_t* hud_strafeguide;
	
public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	void Update(struct playermove_s *ppmove);
};
