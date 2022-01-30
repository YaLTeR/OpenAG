#pragma once
#include <complex>

class CHudStrafeGuide : public CHudBase
{
	double angles[6] = {0.};
	
	std::complex<double> lastSimvel = 0.;
	
	cvar_t* hud_strafeguide;
	cvar_t* hud_strafeguide_zoom;
	cvar_t* hud_strafeguide_height;
	cvar_t* hud_strafeguide_size;
	
public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	void Update(struct ref_params_s *ppmove);
};
