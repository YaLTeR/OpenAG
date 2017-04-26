#include <cmath>
#include <cstring>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "hudgl.h"

int CHudSpeedometer::Init()
{
	m_iFlags = HUD_ACTIVE;

	hud_speedometer = CVAR_CREATE("hud_speedometer", "0", FCVAR_ARCHIVE);

	gHUD.AddHudElem(this);
	return 0;
}

int CHudSpeedometer::VidInit()
{
	return 1;
}

int CHudSpeedometer::Draw(float time)
{
	if (hud_speedometer->value == 0.0f)
		return 0;

	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	gHUD.DrawHudNumberCentered(ScreenWidth / 2, ScreenHeight - 2 * gHUD.m_iFontHeight, speed, r, g, b);

	return 0;
}

void CHudSpeedometer::UpdateSpeed(const float velocity[2])
{
	speed = std::round(std::hypot(velocity[0], velocity[1]));
}
