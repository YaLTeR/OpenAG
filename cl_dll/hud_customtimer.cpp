#include "hud.h"
#include "cl_util.h"

DECLARE_COMMAND(m_CustomTimer, CustomTimer);

int CHudCustomTimer::Init()
{
	HOOK_COMMAND("customtimer", CustomTimer);

	m_iFlags = 0;

	gHUD.AddHudElem(this);
	return 0;
}

int CHudCustomTimer::VidInit()
{
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudCustomTimer::Draw(float time)
{
	if (gHUD.m_flTime >= m_flTurnoffTime || gHUD.m_iIntermission)
	{
		m_iFlags &= ~HUD_ACTIVE;
		gEngfuncs.pfnPlaySoundByName("fvox/bell.wav", 1.0f);
		return 0;
	}

	char str[64];
	int r, g, b;
	auto diff = m_flTurnoffTime - gHUD.m_flTime;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	FillRGBA((ScreenWidth / 2) - 50, (gHUD.m_scrinfo.iCharHeight * 4) - 6, (diff / m_flSeconds) * 100, 4, r, g, b, 210);
	snprintf(str, sizeof(str), "Timer: %d", (int)diff);
	gHUD.DrawHudStringCentered(ScreenWidth / 2, gHUD.m_scrinfo.iCharHeight * 4, str, r, g, b);

	return 0;
}

int CHudCustomTimer::UserCmd_CustomTimer()
{
	if (gEngfuncs.Cmd_Argc() == 2)
	{
		auto time = atof(gEngfuncs.Cmd_Argv(1));
		if (time > 172800)
		{
			gEngfuncs.Con_Printf("Time can't be higher than 172800 seconds (2 days).\n");
			return 1;
		}
		m_flSeconds = time;
		m_flTurnoffTime = gHUD.m_flTime + time;
		m_iFlags |= HUD_ACTIVE;
	}
	else
		gEngfuncs.Con_Printf("customtimer <seconds> - start a local countdown timer\n");

	return 1;
}
