#include <cstdio>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"

extern hud_player_info_t   g_PlayerInfoList [MAX_PLAYERS + 1];
extern extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS + 1];

static char* fmt(const char* fmt, ...)
{
	static char buffer[2048];

	va_list list;
	va_start(list, fmt);

	vsnprintf(buffer, ARRAYSIZE(buffer), fmt, list);

	va_end(list);

	return buffer;
}

int CHudDebug::Init()
{
	hud_debug = CVAR_CREATE("hud_debug", "0", 0);

	m_iFlags = HUD_ACTIVE;

	gHUD.AddHudElem(this);
	return 0;
}

int CHudDebug::VidInit()
{	
	return 1;
}

int CHudDebug::Draw(float time)
{
	if (hud_debug->value == 0.0f)
		return 0;

	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	int y = 20;

	gHUD.DrawHudStringWithColorTags(0, y, "m_sTeamNames:", r, g, b);
	for (int i = 0; i < gViewPort->GetNumberOfTeams(); ++i) {
		gHUD.DrawHudStringWithColorTags(0, y += gHUD.m_scrinfo.iCharHeight, fmt(" %d: `%s`", i + 1, gViewPort->GetTeamName(i + 1)), r, g, b);
	}

	gHUD.DrawHudStringWithColorTags(0, y += gHUD.m_scrinfo.iCharHeight, "g_PlayerExtraInfo teams:", r, g, b);
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		if (g_PlayerExtraInfo[i + 1].teamname[0] == '\0')
			continue;
		
		gHUD.DrawHudStringWithColorTags(0, y += gHUD.m_scrinfo.iCharHeight, fmt(" %d: %hd `%s`", i + 1, g_PlayerExtraInfo[i + 1].teamnumber, g_PlayerExtraInfo[i + 1].teamname), r, g, b);
	}

	return 0;
}
