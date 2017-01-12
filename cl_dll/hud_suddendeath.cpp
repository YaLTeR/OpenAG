#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_SuddenDeath, SuddenDeath);

int CHudSuddenDeath::Init()
{
	HOOK_MESSAGE(SuddenDeath);

	m_iFlags = 0;

	gHUD.AddHudElem(this);
	return 0;
}

int CHudSuddenDeath::VidInit()
{
	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudSuddenDeath::Draw(float time)
{
	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	gHUD.DrawHudStringCentered(ScreenWidth / 2, gHUD.m_scrinfo.iCharHeight * 2, "Sudden death!", r, g, b);

	return 0;
}

int CHudSuddenDeath::MsgFunc_SuddenDeath(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	if (READ_BYTE())
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;

	return 1;
}
