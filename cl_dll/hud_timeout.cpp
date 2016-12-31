#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_Timeout, Timeout);

int CHudTimeout::Init()
{
	HOOK_MESSAGE(Timeout);

	m_State = Inactive;
	m_iTime = 0;
	m_iFlags = 0;

	gHUD.AddHudElem(this);
	return 0;
}

int CHudTimeout::VidInit()
{
	m_State = Inactive;
	return 1;
}

void CHudTimeout::Reset(void)
{
	m_iFlags &= ~HUD_ACTIVE;
}

int CHudTimeout::Draw(float fTime)
{
	if (Inactive == m_State)
	{
		Reset();
		return 1;
	}

	char str[64];
	str[0] = '\0';
	int r, g, b;
	UnpackRGB(r, g, b, RGB_GREENISH);
	if (Called == m_State)
		sprintf(str, "Timeout called, stopping in %d seconds.", m_iTime);
	else if (Countdown == m_State)
		sprintf(str, "Timeout, starting in %d seconds.", m_iTime);
	else
		return 0;

	gHUD.DrawHudStringCentered(ScreenWidth / 2, gHUD.m_scrinfo.iCharHeight * 6, str, r, g, b);

	return 0;
}

int CHudTimeout::MsgFunc_Timeout(const char * name, int size, void * buf)
{
	BEGIN_READ(buf, size);
	m_State = READ_BYTE();
	m_iTime = READ_BYTE();
	m_iFlags |= HUD_ACTIVE;

	return 1;
}
