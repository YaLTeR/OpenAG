#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_NextMap, Nextmap);

int CHudNextMap::Init()
{
	HOOK_MESSAGE(Nextmap);

	m_iFlags = 0;
	next_map[ARRAYSIZE(next_map) - 1] = '\0';

	gHUD.AddHudElem(this);
	return 0;
}

int CHudNextMap::VidInit()
{
	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudNextMap::Draw(float time)
{
	if (gHUD.m_flTime >= draw_until) {
		m_iFlags &= ~HUD_ACTIVE;
		return 0;
	}

	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	char str[ARRAYSIZE(next_map) + 32];
	sprintf(str, "The next map is %s.", next_map);

	gHUD.DrawHudStringCentered(ScreenWidth / 2, gHUD.m_scrinfo.iCharHeight * 5, str, r, g, b);

	return 0;
}

int CHudNextMap::MsgFunc_Nextmap(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);
	strncpy(next_map, READ_STRING(), ARRAYSIZE(next_map) - 1);

	draw_until = gHUD.m_flTime + 10.0f;

	m_iFlags |= HUD_ACTIVE;

	return 1;
}
