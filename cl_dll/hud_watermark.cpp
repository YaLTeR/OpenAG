#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

int CHudWatermark::Init()
{
	m_iFlags = 0;

	gHUD.AddHudElem(this);
	return 0;
}

int CHudWatermark::VidInit()
{
	m_iFlags |= HUD_ACTIVE;
	refresh_draw_until = true;

	return 1;
}

int CHudWatermark::Draw(float time)
{
	if (refresh_draw_until) {
		refresh_draw_until = false;
		draw_until = gHUD.m_flTime + 15.0f;
	}

	if (gHUD.m_flTime >= draw_until) {
		m_iFlags &= ~HUD_ACTIVE;
		return 0;
	}

	int r, g, b;
	UnpackRGB(r, g, b, RGB_YELLOWISH);

	gEngfuncs.pfnDrawString(
		ScreenWidth / 20,
		gHUD.m_scrinfo.iCharHeight,
		"AG-Compat DLL built at " __DATE__ " " __TIME__,
		r,
		g,
		b
	);

	gEngfuncs.pfnDrawString(
		ScreenWidth / 20,
		gHUD.m_scrinfo.iCharHeight * 2,
		"by Ivan \"YaLTeR\" Molodetskikh",
		r,
		g,
		b
	);

	return 0;
}
