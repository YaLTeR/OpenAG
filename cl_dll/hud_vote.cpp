#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_Vote, Vote);

int CHudVote::Init()
{
	HOOK_MESSAGE(Vote);

	m_iFlags = 0;

	str1[ARRAYSIZE(str1) - 1] = '\0';
	str2[ARRAYSIZE(str2) - 1] = '\0';
	called_by[ARRAYSIZE(called_by) - 1] = '\0';

	gHUD.AddHudElem(this);
	return 0;
}

int CHudVote::VidInit()
{
	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudVote::Draw(float time)
{
	if (status == VoteStatus::HIDDEN || gHUD.m_flTime >= draw_until) {
		m_iFlags &= ~HUD_ACTIVE;
		return 0;
	}

	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	char str[72];
	const int x = ScreenWidth / 20;
	int y = ScreenHeight / 8;

	sprintf(str, "Vote: %s %s", str1, str2);
	gEngfuncs.pfnDrawString(x, y, str, r, g, b);
	y += gHUD.m_scrinfo.iCharHeight;

	sprintf(str, "Called by: %s", called_by);
	gHUD.DrawHudStringWithColorTags(x, y, str, r, g, b);
	y += gHUD.m_scrinfo.iCharHeight * 3 / 2;

	switch (status) {
	case VoteStatus::IN_PROGRESS:
		sprintf(str, "For: %hhu", count_for);
		gEngfuncs.pfnDrawString(x, y, str, r, g, b);
		y += gHUD.m_scrinfo.iCharHeight;

		sprintf(str, "Against: %hhu", count_against);
		gEngfuncs.pfnDrawString(x, y, str, r, g, b);
		y += gHUD.m_scrinfo.iCharHeight;

		sprintf(str, "Undecided: %hhu", count_undecided);
		gEngfuncs.pfnDrawString(x, y, str, r, g, b);
		return 0;

	case VoteStatus::ACCEPTED:
		sprintf(str, "Accepted!");
		break;

	case VoteStatus::DENIED:
		sprintf(str, "Denied!");
		break;

	default:
		return 0;
	}

	gEngfuncs.pfnDrawString(x, y, str, r, g, b);

	return 0;
}

int CHudVote::MsgFunc_Vote(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	status = static_cast<VoteStatus>(READ_BYTE());
	count_for = READ_BYTE();
	count_against = READ_BYTE();
	count_undecided = READ_BYTE();

	strncpy(str1, READ_STRING(), ARRAYSIZE(str1) - 1);
	strncpy(str2, READ_STRING(), ARRAYSIZE(str2) - 1);
	strncpy(called_by, READ_STRING(), ARRAYSIZE(called_by) - 1);

	draw_until = gHUD.m_flTime + 4.0f;

	if (status != VoteStatus::HIDDEN)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;

	//gEngfuncs.Con_Printf("Vote: status = %hhu, for = %hhu, against = %hhu, undecided = %hhu, str1 = `%s`, str2 = `%s`, name = `%s`.\n", status, count_for, count_against, count_undecided, str1, str2, n);

	return 1;
}
