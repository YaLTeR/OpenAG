#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_Timeout, Timeout);

int CHudTimeout::Init()
{
	HOOK_MESSAGE(Timeout);

	m_iFlags = 0;

	gHUD.AddHudElem(this);
	return 0;
}

int CHudTimeout::VidInit()
{
	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudTimeout::Draw(float time)
{
	if (status != TimeoutStatus::STARTING && status != TimeoutStatus::IN_PROGRESS)
		return 0;

	char str[64];

	switch (status) {
	case TimeoutStatus::STARTING:
		sprintf(str, "Timeout called, stopping in %hhu seconds.", seconds);
		break;

	case TimeoutStatus::IN_PROGRESS:
		sprintf(str, "Timeout, starting in %hhu seconds.", seconds);
		break;

	default:
		// Unreachable.
		assert(false);
	}

	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	gHUD.DrawHudStringCentered(ScreenWidth / 2, gHUD.m_scrinfo.iCharHeight * 6, str, r, g, b);

	return 0;
}

int CHudTimeout::MsgFunc_Timeout(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	status = static_cast<TimeoutStatus>(READ_BYTE());
	seconds = READ_BYTE();

	if (status != TimeoutStatus::HIDDEN)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;

	// gEngfuncs.Con_Printf("Timeout: status = %hhu; seconds = %hhu.\n", status, seconds);

	return 1;
}
