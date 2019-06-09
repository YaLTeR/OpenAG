#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_PlayerId, PlayerId);

int CHudPlayerId::Init()
{
	HOOK_MESSAGE(PlayerId);

	m_iFlags = 0;
	hud_playerid = CVAR_CREATE("hud_playerid", "1", FCVAR_ARCHIVE);

	gHUD.AddHudElem(this);
	return 0;
}

int CHudPlayerId::VidInit()
{
	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudPlayerId::Draw(float time)
{
	if (gHUD.m_flTime >= draw_until) {
		m_iFlags &= ~HUD_ACTIVE;
		return 0;
	}

	if (hud_playerid->value == 0.0f)
		return 0;

	// Make sure the information is up-to-date.
	gEngfuncs.pfnGetPlayerInfo(player_id, &g_PlayerInfoList[player_id]);

	const auto name = g_PlayerInfoList[player_id].name;
	if (!name) {
		m_iFlags &= ~HUD_ACTIVE;
		return 0;
	}

	int r, g, b;
	UnpackRGB(r, g, b, teammate ? RGB_GREENISH : RGB_REDISH);

	char str[64];

	if (teammate)
		sprintf(str, "%s^0 %d/%d", name, health, armor);
	else
		sprintf(str, "%s", name);

	if (CVAR_GET_FLOAT("hud_centerid"))
		gHUD.DrawHudStringCenteredWithColorTags(ScreenWidth / 2,
		                                        ScreenHeight - ScreenHeight / 4,
		                                        str,
		                                        r,
		                                        g,
		                                        b);
	else
		gHUD.DrawHudStringWithColorTags(10, ScreenHeight - ScreenHeight / 8, str, r, g, b);

	return 0;
}

int CHudPlayerId::MsgFunc_PlayerId(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	player_id = READ_BYTE();
	teammate = (READ_BYTE() == 1);
	health = READ_SHORT();
	armor = READ_SHORT();

	draw_until = gHUD.m_flTime + 2.0f;

	m_iFlags |= HUD_ACTIVE;

	return 1;
}
