#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "update_checker.h"

extern cvar_t* hud_watermark;

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
	update_is_available = update_checker::is_update_available();

	return 1;
}

int CHudWatermark::Draw(float time)
{
	float duration = 15.0f;
	if (hud_watermark->value != 1)
		duration = 5.0f;
		
	if (refresh_draw_until || (draw_until > gHUD.m_flTime + duration)) {
		refresh_draw_until = false;
		draw_until = gHUD.m_flTime + duration;
	}

	if (gHUD.m_flTime >= draw_until) {
		m_iFlags &= ~HUD_ACTIVE;
		return 0;
	}

	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	if (hud_watermark->value != 0) {
		gEngfuncs.pfnDrawString(ScreenWidth / 20, gHUD.m_scrinfo.iCharHeight, "OpenAG client build " __DATE__, r, g, b);
		gEngfuncs.pfnDrawString(ScreenWidth / 20, gHUD.m_scrinfo.iCharHeight * 2, "j.mp/OpenAG", r, g, b);
	}

	if (update_is_available)
		gEngfuncs.pfnDrawString(ScreenWidth / 20, gHUD.m_scrinfo.iCharHeight / 2 * 7, "An update is available.", r, g, b);

	return 0;
}
